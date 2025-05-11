#pragma once

#include <3ds.h>

#define CONTROLLER_PACKET_MAGIC (0x33434333) // 3CC3
#define CONTROLLER_PACKET_STICK_MAX (0x7FFF)
#define CONTROLLER_INPUT_STICK_MAX (0x9C)
#define CONTROLLER_PACKET_VECTOR_PRECISION (10000000.0f)

typedef struct {
	s32 dx;
	s32 dy;
} ControllerPacketCircle;

typedef struct {
	s32 x;
	s32 y;
	s32 z;
} ControllerPacketVector;

typedef struct {
	s32 magic;
	u64 tick;
	u32 kHeld;
	u32 attrs;
	ControllerPacketCircle cLeft;
	ControllerPacketCircle cRight;
	ControllerPacketVector accel;
	ControllerPacketVector angle;
} ControllerPacket;

enum {
	CONTROLLER_PACKET_VKEY_LSBTN = BIT(12),
	CONTROLLER_PACKET_VKEY_RSBTN = BIT(13),
	CONTROLLER_PACKET_VKEY_EXIT = BIT(17),
	CONTROLLER_PACKET_VKEY_CAPTURE = BIT(18),
	CONTROLLER_PACKET_VKEY_HOME = BIT(19),
};

enum {
	CONTROLLER_PACKET_ATTR_ACCEL = BIT(0),
	CONTROLLER_PACKET_ATTR_ANGLE = BIT(1)
};

void controllerPacketDump(const ControllerPacket* packet);
void controllerPacketRead(ControllerPacket* out);
