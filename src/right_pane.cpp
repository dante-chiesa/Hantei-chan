#include "right_pane.h"
#include "frame_disp.h"
#include <imgui.h>

void RightPane::Draw()
{	
	ImGui::Begin("Right Pane", 0);
	auto seq = frameData->get_sequence(currState.pattern);
	if(seq)
	{
		int nframes = seq->frames.size() - 1;
		if(nframes >= 0)
		{
			Frame &frame = seq->frames[currState.frame];
			if (ImGui::TreeNode("Attack data"))
			{
				AtDisplay(&frame.AT);
				if(im::Button("Copy AT")){
					copiedAt = frame.AT;
				}

				im::SameLine(0,20.f); 
				if(im::Button("Paste AT")){
					frame.AT = copiedAt;
				}

				ImGui::TreePop();
				ImGui::Separator();
			}
			if(ImGui::TreeNode("Effects"))
			{
				EfDisplay(&frame.EF, &copiedEf);
				if(im::Button("Copy all")){
					copiedEfList = frame.EF;
				}
				im::SameLine(0,20.f); 
				if(im::Button("Paste all")){
					frame.EF = copiedEfList;
				}
				im::SameLine(0,20.f); 
				if(im::Button("Add copy")){
					frame.EF.push_back(copiedEf);
				}
				ImGui::TreePop();
				ImGui::Separator();
			}
			if(ImGui::TreeNode("Conditions"))
			{
				IfDisplay(&frame.IF, &copiedIf);
				if(im::Button("Copy all")){
					copiedIfList = frame.IF;
				}
				im::SameLine(0,20.f); 
				if(im::Button("Paste all")){
					frame.IF = copiedIfList;
				}
				im::SameLine(0,20.f); 
				if(im::Button("Add copy")){
					frame.IF.push_back(copiedIf);
				}
				ImGui::TreePop();
				ImGui::Separator();
			}
		}
	}
	ImGui::End();
}

