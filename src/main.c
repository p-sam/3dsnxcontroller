#include <3ds.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "sockinput.h"
#include "backlight.h"
#include "controller_packet.h"

#define ADDR_FILE_PATH TARGET ".addr.txt"
#define SOC_ALIGN 0x1000
#define SOC_BUFFERSIZE 0x100000

#define CONSOLE_AT(x, y) "\x1b[" #x ";" #y "H"

static PrintConsole g_topConsole;
static PrintConsole g_bottomConsole;

static void drawHorizontalLine(PrintConsole* console, unsigned int y) {
	printf(CONSOLE_AT(%d, 1), y);
	for(int i = 0; i < console->consoleWidth; i++) {
		printf("_");
	}
}

static void drawVerticalSeparator(unsigned int yStart, unsigned int length, unsigned int x) {
	for(int i = 0; i < length; i++) {
		printf(CONSOLE_AT(%d, %d) "|", yStart + i, x);
	}
}

static void drawBottomScreen() {
	consoleSelect(&g_bottomConsole);

	printf(CONSOLE_CYAN);
	printf(CONSOLE_AT(1, 12) "| Touch to exit |\n");
	printf(CONSOLE_AT(2, 12) "|_______________|\n");

	drawHorizontalLine(&g_bottomConsole, g_bottomConsole.consoleHeight - 5);
	drawVerticalSeparator(g_bottomConsole.consoleHeight - 4, 5, g_bottomConsole.consoleWidth / 2);

	printf(CONSOLE_AT(%d, %d) "CAPTURE", g_bottomConsole.consoleHeight - 2, g_bottomConsole.consoleWidth / 2 - 8);
	printf(CONSOLE_AT(%d, %d) "HOME", g_bottomConsole.consoleHeight - 2, g_bottomConsole.consoleWidth / 2 + 2);

	drawHorizontalLine(&g_bottomConsole, g_bottomConsole.consoleHeight - 12);

	drawVerticalSeparator(g_bottomConsole.consoleHeight - 11, 7, 14);
	drawVerticalSeparator(g_bottomConsole.consoleHeight - 11, 7, 26);

	printf(CONSOLE_AT(%d, %d) "LEFT STICK", g_bottomConsole.consoleHeight - 9, 2);
	printf(CONSOLE_AT(%d, %d) "BUTTON", g_bottomConsole.consoleHeight - 7, 2);

	printf(CONSOLE_AT(%d, %d) "RIGHT STICK", g_bottomConsole.consoleHeight - 9, g_bottomConsole.consoleWidth - 12);
	printf(CONSOLE_AT(%d, %d) "BUTTON", g_bottomConsole.consoleHeight - 7, g_bottomConsole.consoleWidth - 7);

	printf(CONSOLE_AT(%d, %d) "L+R STICK", g_bottomConsole.consoleHeight - 9, 16);
	printf(CONSOLE_AT(%d, %d) "BUTTONS", g_bottomConsole.consoleHeight - 7, 17);

	printf(CONSOLE_RESET);
	consoleSelect(&g_topConsole);
}

static void updateScreens() {
	// Flush and swap framebuffers
	gfxFlushBuffers();
	gfxSwapBuffers();

	gspWaitForVBlank();
}

static bool loadSockAddr(struct sockaddr_in* out) {
	char buf[SOCKINPUT_STRLEN] = {0};

	FILE* f = fopen(ADDR_FILE_PATH, "r");
	if(f) {
		fread(buf, 1, sizeof(buf), f);
		buf[SOCKINPUT_STRLEN-1] = '\0';
		fclose(f);
	}

	if(!sockInputAsk(out, buf)) {
		return false;
	}

	f = fopen(ADDR_FILE_PATH, "w");
	if(f) {
		if(sockInputFormatToString(out, buf, sizeof(buf))) {
			fprintf(f, "%s", buf);
		}
		fclose(f);
	}
	return true;
}

int main(int argc, char **argv) {
	gfxInitDefault();
	gfxSetDoubleBuffering(GFX_TOP, false);
	gfxSetDoubleBuffering(GFX_BOTTOM, false);

	consoleInit(GFX_TOP, &g_topConsole);
	consoleInit(GFX_BOTTOM, &g_bottomConsole);

	consoleSelect(&g_topConsole);

	printf(CONSOLE_GREEN "%s %s" CONSOLE_RESET "\n\n", APP_TITLE, APP_VERSION);
	printf("Init services...\n");
	socInit((u32*)memalign(SOC_ALIGN, SOC_BUFFERSIZE), SOC_BUFFERSIZE);

	int sock;
	struct sockaddr_in saDest;
	char addrDisplay[SOCKINPUT_STRLEN];
	printf("Loading settings...\n");

	bool ready = loadSockAddr(&saDest) && sockInputFormatToString(&saDest, addrDisplay, sizeof(addrDisplay));
	if(!ready) {
		printf("Failed to load settings!\n");
	}

	if(ready) {
		printf("Sending inputs to %s...\n", addrDisplay);
		sock = socket(AF_INET, SOCK_DGRAM, 0);

		struct sockaddr_in saIn;
		saIn.sin_addr.s_addr = INADDR_ANY;
		bind(sock, (struct sockaddr *)&saIn, sizeof(saIn));
	}

#ifdef DEBUG_PRINT_STATE
	printf("sizeof(ControllerPacket): %u\n", sizeof(ControllerPacket));
#endif

	drawBottomScreen();
	updateScreens();
	backlightStore();

	ControllerPacket packet;
	memset(&packet, 0, sizeof(packet));

	while (aptMainLoop()) {
		controllerPacketRead(&packet);

		if(ready) {
			sendto(sock, &packet, sizeof(packet), 0, (const struct sockaddr *)&saDest, sizeof(saDest));
		}

#ifdef DEBUG_PRINT_STATE
		consoleSelect(&g_bottomConsole);
		printf(CONSOLE_AT(5, 1));
		controllerPacketDump(&packet);
		updateScreens();
		consoleSelect(&g_topConsole);
#else
		if(ready) {
			backlightDim();
		}
		svcSleepThread(5000000ULL);
#endif

		if(packet.kHeld & CONTROLLER_PACKET_VKEY_EXIT) {
			break;
		}
	}

#ifndef DEBUG_PRINT_STATE
	if(ready) {
		backlightReset();
	}
#endif

	socExit();
	gfxExit();
	return 0;
}
