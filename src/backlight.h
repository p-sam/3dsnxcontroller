#pragma once

#include <3ds.h>

// @see http://4dsdev.kuribo64.net/thread.php?user=102

#ifndef REG_LCDBACKLIGHTMAIN
#define REG_LCDBACKLIGHTMAIN (u32)(0x1ED02240 - 0x1EB00000)
#endif

#ifndef REG_LCDBACKLIGHTSUB
#define REG_LCDBACKLIGHTSUB (u32)(0x1ED02A40 - 0x1EB00000)
#endif

void backlightStore();
void backlightDim();
void backlightReset();
