#ifndef PARTS_H_GUARD
#define PARTS_H_GUARD

#include "image.h"
#include <vector>

class Parts{
private:
	char *data = nullptr;

	struct tx{
		char *name;
		char *data;
		int w,h;
		int bpp;
		int type;
	};
	std::vector<tx> textures;


	unsigned int *MainLoad(unsigned int *data, const unsigned int *data_end);
	unsigned int *P_Load(unsigned int *data, const unsigned int *data_end);
	unsigned int *PrLoad(unsigned int *data, const unsigned int *data_end);
	unsigned int *PpLoad(unsigned int *data, const unsigned int *data_end);
	unsigned int *PgLoad(unsigned int *data, const unsigned int *data_end);
	
public:
	bool Load(const char *name);

	ImageData *GetTexture(unsigned int n);
};

#endif /* PARTS_H_GUARD */
