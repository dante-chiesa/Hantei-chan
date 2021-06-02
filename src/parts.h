#ifndef PARTS_H_GUARD
#define PARTS_H_GUARD

#include "image.h"
#include <vector>

class Parts{
private:
	char *data = nullptr;

	struct PatternPart
	{
		char *name;
		int uv[4]; //x,y,w,h TLBR
		int xy[2];
		int wh[2];
		int texture;
		int type;
		int jump;
	};

	struct PartGfx{
		char *name;
		char *data;
		int w,h;
		int bpp;
		int type;
	};
	std::vector<PartGfx> textures;


	unsigned int *MainLoad(unsigned int *data, const unsigned int *data_end);
	unsigned int *P_Load(unsigned int *data, const unsigned int *data_end);
	unsigned int *PrLoad(unsigned int *data, const unsigned int *data_end);
	unsigned int *PpLoad(unsigned int *data, const unsigned int *data_end);
	unsigned int *PgLoad(unsigned int *data, const unsigned int *data_end);
	unsigned int *VeLoad(unsigned int *data, const unsigned int *data_end, int amount);
	
public:
	bool Load(const char *name);

	ImageData *GetTexture(unsigned int n);
};

#endif /* PARTS_H_GUARD */
