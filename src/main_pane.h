#ifndef MAINPANE_H_GUARD
#define MAINPANE_H_GUARD
#include "draw_window.h"
#include "framedata.h"
#include "render.h"
#include <string>

//This is the main pane on the left
class MainPane : DrawWindow
{
public:
	MainPane(Render* render, FrameData *frameData, FrameState &fs);
	void Draw();

	void RegenerateNames();

private:
	Sequence copiedPattern{};
	bool copyThisFrame = true;
	std::string *decoratedNames;

	bool rangeWindow = false;
	int ranges[2]{};

};

#endif /* MAINPANE_H_GUARD */
