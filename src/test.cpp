#include <filesystem>
#include <fstream>
#include <iostream>
#include "framedata.h"
#include "parts.h"


void TestHa6(const char *where)
{
	namespace fs = std::filesystem;

	FrameData fd;

	std::string folderIn = where;
	for(const fs::directory_entry &file : fs::directory_iterator(folderIn))
	{

		std::string filename = file.path().filename().string();
		if(filename.back() == '6' ) //Ending in ha6
		{
			fd.load((folderIn + filename).c_str());
		}
	}

	std::cout<<"Max:"<<maxCount<<"\n";
	std::cout<<"Set:\n";
	for(int number : numberSet)
	{
		std::cout <<number<<"\n";
	}
}

void TestPat(const char *where)
{

	namespace fs = std::filesystem;
	Parts pat;

	std::string folderIn = where;
	for(const fs::directory_entry &file : fs::directory_iterator(folderIn))
	{

		std::string filename = file.path().filename().string();
		if(file.path().extension().compare(".pat") == 0) //Ending in .pat
		{
			pat.Load((folderIn + filename).c_str());
		}
	}
}