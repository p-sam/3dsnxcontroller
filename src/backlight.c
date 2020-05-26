#include "backlight.h"

#define BACKLIGHT_LEVEL 5
static u32 g_brightnessRegMain;
static u32 g_brightnessRegSub;

void backlightStore(void) {
	GSPGPU_ReadHWRegs(REG_LCDBACKLIGHTMAIN, &g_brightnessRegMain, 4);
	GSPGPU_ReadHWRegs(REG_LCDBACKLIGHTSUB, &g_brightnessRegSub, 4);
}

void backlightDim(void) {
	u32 main;
	u32 sub;
	GSPGPU_ReadHWRegs(REG_LCDBACKLIGHTMAIN, &main, 4);
	GSPGPU_ReadHWRegs(REG_LCDBACKLIGHTSUB, &sub, 4);

	if(main != BACKLIGHT_LEVEL || sub != BACKLIGHT_LEVEL) {
		main = BACKLIGHT_LEVEL;
		GSPGPU_WriteHWRegs(REG_LCDBACKLIGHTMAIN, &main, 4);
		GSPGPU_WriteHWRegs(REG_LCDBACKLIGHTSUB, &main, 4);
	}

}

void backlightReset(void) {
	GSPGPU_WriteHWRegs(REG_LCDBACKLIGHTMAIN, &g_brightnessRegMain, 4);
	GSPGPU_WriteHWRegs(REG_LCDBACKLIGHTSUB, &g_brightnessRegSub, 4);
}
