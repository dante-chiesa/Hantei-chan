#ifndef INI_H_GUARD
#define INI_H_GUARD
#include "framedata.h"
#include "cg.h"

extern struct Settings
{
	//default
	float color[3]{0.324f, 0.409f, 0.185f};
	int zoomLevel = 3;
	bool bilinear = 0;
	int theme = 0;
	float fontSize = 18.f;
	short posX = 100;
	short posY = 100;
	short winSizeX = 1280;
	short winSizeY = 800;
	bool maximized = false;
	bool idleUpdate = true;
} gSettings;

bool LoadFromIni(FrameData *framedata, CG *cg, const std::string& iniPath);
void InitIni();

#endif /* INI_H_GUARD */
