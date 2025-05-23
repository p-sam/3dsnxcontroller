#include "controller_packet.h"
#include <stdio.h>

static char* g_keysNames[] = {
	"A", "B", "SELECT", "START",
	"DRIGHT", "DLEFT", "DUP", "DDOWN",
	"R", "L", "X", "Y",
	"LSBTN", "RSBTN", "ZL", "ZR",
	NULL, "EXIT", "CAPTURE", "HOME",
	"TOUCH"
};

void controllerPacketDump(const ControllerPacket* packet) {
	printf("kHeld:");
	for (u32 i = 0; i < (sizeof(g_keysNames) / sizeof(g_keysNames[0])); i++) {
		if(i % 4 == 0) {
			printf("\n\t");
		}

		if(g_keysNames[i] == NULL) {
			continue;
		}

		if(packet->kHeld & BIT(i)) {
			printf("\x1b[47;30m%s\x1b[0m ", g_keysNames[i]);
		} else {
			printf("%s ", g_keysNames[i]);
		}
	}
	printf("\n");

	printf("cLeft : x=%06ld; y=%06ld\n", packet->cLeft.dx, packet->cLeft.dy);
	printf("cRight: x=%06ld; y=%06ld\n", packet->cRight.dx, packet->cRight.dy);

	touchPosition touchPos;
	hidTouchRead(&touchPos);
	printf("touch : x=%04d; y=%04d\n", touchPos.px, touchPos.py);
}

static inline void _circleToPacket(circlePosition* in, ControllerPacketCircle* out) {
	out->dx = (s32)(in->dx) * CONTROLLER_PACKET_STICK_MAX / CONTROLLER_INPUT_STICK_MAX;
	out->dy = (s32)(in->dy) * CONTROLLER_PACKET_STICK_MAX / CONTROLLER_INPUT_STICK_MAX;
}

void controllerPacketRead(ControllerPacket* out) {
	hidScanInput();

	touchPosition touchPos;
	hidTouchRead(&touchPos);

	out->magic = CONTROLLER_PACKET_MAGIC;
	out->tick = svcGetSystemTick();
	out->kHeld = hidKeysHeld();

	circlePosition circlePos;
	hidCircleRead(&circlePos);
	_circleToPacket(&circlePos, &out->cLeft);
	hidCstickRead(&circlePos);
	_circleToPacket(&circlePos, &out->cRight);

	if(out->kHeld & KEY_TOUCH) {
		if(touchPos.py <= 20) {
			if(touchPos.px >= 90 && touchPos.px <= 220) {
				out->kHeld |= CONTROLLER_PACKET_VKEY_EXIT;
			}
		} else if (touchPos.py >= 200) {
			if(touchPos.px < 157) {
				out->kHeld |= CONTROLLER_PACKET_VKEY_CAPTURE;
			} else {
				out->kHeld |= CONTROLLER_PACKET_VKEY_HOME;
			}
		} else if (touchPos.py >= 145) {
			if(touchPos.px <= 205) {
				out->kHeld |= CONTROLLER_PACKET_VKEY_LSBTN;
			}
			if(touchPos.px >= 110) {
				out->kHeld |= CONTROLLER_PACKET_VKEY_RSBTN;
			}
		}
	}
}

