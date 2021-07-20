#include "filedialog.h"
#include "main.h"
#include <commdlg.h>

std::string FileDialog(int fileType, bool save)
{
	OPENFILENAMEA ofn;       // common dialog box structure
	char szFile[260];       // buffer for file name
	HANDLE hf;              // file handle

	// Initialize OPENFILENAME
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = mainWindowHandle;
	ofn.lpstrFile = szFile;
	// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
	// use the contents of szFile to initialize itself.
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	if(fileType == fileType::HA6)
	{
		ofn.lpstrFilter = "Hantei 6 files (*.ha6)\0*.ha6\0All\0*.*\0";
	}
	else if(fileType == fileType::CG)
	{
		ofn.lpstrFilter = "Graphics files (*.cg)\0*.cg\0All\0*.*\0";
	}
	else if(fileType == fileType::PAL)
	{
		ofn.lpstrFilter = "Palette files (*.pal)\0*.pal\0All\0*.*\0";
	}
	else if(fileType == fileType::TXT)
	{
		ofn.lpstrFilter = "INI text files (*.txt)\0*.txt\0All\0*.*\0";
	}
	else if(fileType == fileType::PAT)
	{
		ofn.lpstrFilter = "Parts (*.pat)\0*.pat\0All\0*.*\0";
	}
	else if(fileType == fileType::CHR)
	{
		ofn.lpstrFilter = "Character (*.char)\0*.char\0All\0*.*\0";
	}
	else
	{
		ofn.lpstrFilter = "All\0*.*\0";
	}
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = ".";

	if(!save)
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	// Display the Open dialog box. 
	if(save)
	{
		if(GetSaveFileNameA(&ofn)==TRUE)
			return ofn.lpstrFile;
	}
	else if (GetOpenFileNameA(&ofn)==TRUE)
		return ofn.lpstrFile;
	return {};
}