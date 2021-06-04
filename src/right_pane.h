#ifndef RIGHTPANEASDASD_H_GUARD
#define RIGHTPANEASDASD_H_GUARD
#include "draw_window.h"
#include "framedata.h"

//This is the main pane on the left
class RightPane : DrawWindow
{
	using DrawWindow::DrawWindow;
	
	
public:
	void Draw();

private:
	Frame_AT copiedAt{};
	std::vector<Frame_IF> copiedIfList;
	std::vector<Frame_EF> copiedEfList;
	Frame_IF copiedIf{};
	Frame_EF copiedEf{};
};

#endif /* RIGHTPANEASDASD_H_GUARD */
