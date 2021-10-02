#ifndef ABOUT_H_GUARD
#define ABOUT_H_GUARD

#include "draw_window.h"
#include <imgui.h>

#ifndef HA6GUIVERSION
#define HA6GUIVERSION " custom"
#endif

//This is the main pane on the left
class AboutWindow 
{
public:
	bool isVisible = false;

	void Draw()
	{
		if(isVisible)
		{
			ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(1,1,1,0.3));
			ImGui::SetNextWindowSize(ImVec2{400,155});
			ImGui::Begin("About", &isVisible, ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0,0,0,1));
			ImGui::Text("Hantei-chan v" HA6GUIVERSION);
			ImGui::Separator();
			ImGui::Text("Made by omanko.");
			ImGui::Spacing();
			ImGui::Text("Special thanks to mauve, MadScientist, u4ick and Rhekar.");
			ImGui::Separator();
			ImGui::Text("\nApplication average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();
			ImGui::PopStyleColor(2);
		}
	}
};


class HelpWindow 
{
public:
	bool isVisible = false;

	void Draw()
	{
		if(isVisible)
		{
			ImGui::SetNextWindowSize(ImVec2{375,305});
			ImGui::Begin("Shortcuts", &isVisible, ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

			constexpr const char *shortcutText[] = {
				"Up", "Next pattern",
				"Down", "Previous pattern",
				"Right", "Next frame",
				"Left", "Previous frame",
				"X", "Next box",
				"Z", "Previous box",
				"P", "Output current view as image",
				"L", "Toggle axis",
				"F", "Hide user interface",
				"-","-",
				"Right click & drag", "Draw box",
				"Left click & drag", "Pan view",
			};

			if (ImGui::BeginTable("table1", 2))
			{
				for (int row = 0; row < sizeof(shortcutText)/sizeof(char*)/2; row++)
				{
					ImGui::TableNextRow();
					for (int column = 0; column < 2; column++)
					{
						ImGui::TableSetColumnIndex(column);
						ImGui::Text("%s", shortcutText[row*2 + column]);
					}
				}
				ImGui::EndTable();
			}

			ImGui::End();
		}
	}

private:
};


#endif /* ABOUT_H_GUARD */
