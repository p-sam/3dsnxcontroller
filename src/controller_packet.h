#pragma once

#include <3ds.h>

#define CONTROLLER_PACKET_MAGIC 0x33434332 // 3CC2

typedef struct {
	u32 magic;
	u64 tick;
	u32 kHeld;
	circlePosition cLeft;
	circlePosition cRight;
} ControllerPacket;

enum {
	CONTROLLER_PACKET_VKEY_LSBTN = BIT(12),
	CONTROLLER_PACKET_VKEY_RSBTN = BIT(13),
	CONTROLLER_PACKET_VKEY_EXIT = BIT(17),
	CONTROLLER_PACKET_VKEY_CAPTURE = BIT(18),
	CONTROLLER_PACKET_VKEY_HOME = BIT(19),
};

void controllerPacketDump(const ControllerPacket* packet);
void controllerPacketRead(ControllerPacket* out);
