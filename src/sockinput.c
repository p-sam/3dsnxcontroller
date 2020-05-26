#include "sockinput.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

bool sockInputFormatFromString(const char* input, struct sockaddr_in* out) {
	char buf[SOCKINPUT_STRLEN];
	struct sockaddr_in sa;

	strncpy(buf, input, SOCKINPUT_STRLEN);
	buf[SOCKINPUT_STRLEN-1] = '\0';

	char* ipStr = buf;
	char* portStr = strstr(buf, ":");
	if(portStr == NULL || *portStr == '\0') {
		return false;
	}

	*portStr = '\0';
	portStr++;

	char* endptr;
	sa.sin_port = htons(strtoul(portStr, &endptr, 10));
	if(*endptr != '\0' || sa.sin_port == 0 || sa.sin_port > SOCKINPUT_MAXPORT) {
		return false;
	}

	if(inet_pton(AF_INET, ipStr, &(sa.sin_addr)) != 1) {
		return false;
	}

	if(out != NULL) {
		sa.sin_family = AF_INET;
		memcpy(out, &sa, sizeof(struct sockaddr_in));
	}

	return true;
}

bool sockInputFormatToString(const struct sockaddr_in* in, char* out, size_t size) {
	char buf[INET_ADDRSTRLEN];

	if(inet_ntop(AF_INET, &(in->sin_addr), buf, sizeof(buf)) == NULL) {
		return false;
	}

	return snprintf(out, size, "%s:%u", buf, ntohs(in->sin_port)) > 0;
}

static SwkbdCallbackResult _sockInputSwkbdCallback(void* user, const char** ppMessage, const char* text, size_t textlen) {
	if(!sockInputFormatFromString(text, (struct sockaddr_in*)user)) {
		*ppMessage = "Input is not a valid ip:port combo";
		return SWKBD_CALLBACK_CONTINUE;
	}

	return SWKBD_CALLBACK_OK;
}

bool sockInputAsk(struct sockaddr_in* out, const char* initialStr) {
	SwkbdState swkbd;
	char buf[SOCKINPUT_STRLEN];

	swkbdInit(&swkbd, SWKBD_TYPE_NUMPAD, 2, SOCKINPUT_STRLEN-1);
	swkbdSetValidation(&swkbd, SWKBD_NOTBLANK_NOTEMPTY, 0, 0);
	swkbdSetNumpadKeys(&swkbd, '.', ':');
	swkbdSetFilterCallback(&swkbd, _sockInputSwkbdCallback, out);
	swkbdSetHintText(&swkbd, "ip:port");
	if(initialStr != NULL) {
		swkbdSetInitialText(&swkbd, initialStr);
	}

	SwkbdButton button = swkbdInputText(&swkbd, buf, sizeof(buf));

	return button == SWKBD_BUTTON_RIGHT;
}
