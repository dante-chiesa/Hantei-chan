#include "main_pane.h"
#include "pattern_disp.h"
#include "frame_disp.h"
#include <imgui.h>	

MainPane::MainPane(Render* render, FrameData *framedata, FrameState &fs) : DrawWindow(render, framedata, fs),
decoratedNames(nullptr)
{
	
}

void MainPane::RegenerateNames()
{
	delete[] decoratedNames;
	
	if(frameData && frameData->m_loaded)
	{
		decoratedNames = new std::string[frameData->get_sequence_count()];
		int count = frameData->get_sequence_count();

		for(int i = 0; i < count; i++)
		{
			decoratedNames[i] = frameData->GetDecoratedName(i);
		}
	}
	else
		decoratedNames = nullptr;
}

void MainPane::Draw()
{	
	namespace im = ImGui;
	im::Begin("Left Pane",0);
	if(frameData->m_loaded)
	{
		if (im::BeginCombo("Pattern", decoratedNames[currState.pattern].c_str(), ImGuiComboFlags_HeightLargest))
		{
			auto count = frameData->get_sequence_count();
			for (int n = 0; n < count; n++)
			{
				const bool is_selected = (currState.pattern == n);
				if (im::Selectable(decoratedNames[n].c_str(), is_selected))
				{
					currState.pattern = n;
					currState.frame = 0;
				}

				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if (is_selected)
					im::SetItemDefaultFocus();
			}
			im::EndCombo();
		}
		auto seq = frameData->get_sequence(currState.pattern);
		if(seq)
		{
			int nframes = seq->frames.size() - 1;
			if(nframes >= 0)
			{			
				float spacing = im::GetStyle().ItemInnerSpacing.x;
				im::SetNextItemWidth(im::GetWindowWidth() - 160.f);
				im::SliderInt("##frameSlider", &currState.frame, 0, nframes);
				im::SameLine();
				im::PushButtonRepeat(true);
				if(im::ArrowButton("##left", ImGuiDir_Left))
					currState.frame--;
				im::SameLine(0.0f, spacing);
				if(im::ArrowButton("##right", ImGuiDir_Right))
					currState.frame++;
				im::PopButtonRepeat();
				im::SameLine();
				im::Text("%d/%d", currState.frame+1, nframes+1);

				if(currState.frame < 0)
					currState.frame = 0;
				else if(currState.frame > nframes)
					currState.frame = nframes;
				
				if(im::Button("Animate"))
				{
					currState.animating = !currState.animating;
					currState.animeSeq = currState.pattern;
				}
			}
			else
			{
				im::Text("This pattern has no frames.");
				if(im::Button("Add frame"))
				{
					seq->frames.push_back({});
					currState.frame = 0;
				}
				
			}

			im::BeginChild("FrameInfo", {0, im::GetWindowSize().y-im::GetFrameHeight()*4-8}, false, ImGuiWindowFlags_HorizontalScrollbar);
			if (im::TreeNode("Pattern data"))
			{
				if(im::InputText("Pattern name", &seq->name))
				{
					decoratedNames[currState.pattern] = frameData->GetDecoratedName(currState.pattern);
				}
				PatternDisplay(seq);
				if(im::Button("Copy pattern")){
					copiedPattern = *seq;
				}
				im::SameLine(0,20.f); 
				if(im::Button("Paste pattern")){
					*seq = copiedPattern;
					decoratedNames[currState.pattern] = frameData->GetDecoratedName(currState.pattern);
				}

				if(im::Button("Push pattern copy"))
				{
					patCopyStack.push_back(SequenceWId{currState.pattern, *seq});
				}
				im::SameLine(0,20.f);
				if(im::Button("Pop all and paste"))
				{
					PopCopies();
					RegenerateNames();
				}
				im::SameLine(0,20.f);
				im::Text("%llu copies", patCopyStack.size());

				im::TreePop();
				im::Separator();
			}
			if(nframes >= 0)
			{
				Frame &frame = seq->frames[currState.frame];
				if(im::TreeNode("State data"))
				{
					AsDisplay(&frame.AS);
					im::TreePop();
					im::Separator();
				}
				if (im::TreeNode("Animation data"))
				{
					AfDisplay(&frame.AF);
					im::TreePop();
					im::Separator();
				}
				if (im::TreeNode("Tools"))
				{
					im::Checkbox("Make copy current frame", &copyThisFrame);
					
					if(im::Button("Append frame"))
					{
						if(copyThisFrame)
							seq->frames.push_back(frame);
						else
							seq->frames.push_back({});
					}

					im::SameLine(0,20.f); 
					if(im::Button("Insert frame"))
					{
						if(copyThisFrame)
							seq->frames.insert(seq->frames.begin()+currState.frame, frame);
						else
							seq->frames.insert(seq->frames.begin()+currState.frame, {});
					}

					im::SameLine(0,20.f);
					if(im::Button("Delete frame"))
					{
						seq->frames.erase(seq->frames.begin()+currState.frame);
						if(currState.frame >= seq->frames.size())
							currState.frame--;
					}

					if(im::Button("Range paste"))
					{
						ranges[0] = 0;
						ranges[1] = 0;
						rangeWindow = !rangeWindow;
					}
					
					im::TreePop();
					im::Separator();
				}
			}
			else
				rangeWindow = false;
			im::EndChild();
		}
	}
	else
		im::Text("Load some data first.");

	im::End();

	if(rangeWindow)
	{
		im::SetNextWindowSize(ImVec2{400,120}, ImGuiCond_FirstUseEver);
		im::Begin("Range paste", &rangeWindow);
		im::InputInt2("Range of frames", ranges);
		if(im::Button("Paste color"))
		{
			auto seq = frameData->get_sequence(currState.pattern);
			if(ranges[0] == ranges[1])
				ranges[1] = seq->frames.size()-1;

			for(int i = ranges[0]; i <= ranges[1] && i >= 0 && i < seq->frames.size(); i++)
			{
				memcpy(seq->frames[i].AF.rgba, seq->frames[currState.frame].AF.rgba, sizeof(float)*4);
				seq->frames[i].AF.blend_mode = seq->frames[currState.frame].AF.blend_mode;
			}
		}
		if(im::Button("Paste transform"))
		{
			auto seq = frameData->get_sequence(currState.pattern);
			if(ranges[0] == ranges[1])
				ranges[1] = seq->frames.size()-1;
			
			for(int i = ranges[0]; i <= ranges[1] && i >= 0 && i < seq->frames.size(); i++)
			{
				seq->frames[i].AF.offset_x = seq->frames[currState.frame].AF.offset_x;
				seq->frames[i].AF.offset_y = seq->frames[currState.frame].AF.offset_y;
				memcpy(seq->frames[i].AF.scale, seq->frames[currState.frame].AF.scale, sizeof(float)*2);
				memcpy(seq->frames[i].AF.rotation, seq->frames[currState.frame].AF.rotation, sizeof(float)*3);
			}
		}
		im::End();
	}
}

void MainPane::PopCopies()
{
	for(auto &pat : patCopyStack)
	{
		*frameData->get_sequence(pat.id) = pat.seq;
	}
	patCopyStack.clear();
}