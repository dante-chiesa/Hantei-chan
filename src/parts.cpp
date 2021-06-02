#include "parts.h"
#include "misc.h"

#include <iostream>
#include <cassert>

//UNI only
unsigned int* Parts::VeLoad(unsigned int *data, const unsigned int *data_end, int amount) 
{
	while (data < data_end) {
		unsigned int *buf = data;
		++data;
		
		if (!memcmp(buf, "VNST", 4)) {
			data += 8*amount;
		} else if (!memcmp(buf, "VEED", 4)) {
			break;
		} else{
			char tag[5]{};
			memcpy(tag,buf,4);
			std::cout <<"\tUnknown VE level tag: " << tag <<"\n";
		}
	}
	return data;
}

unsigned int* Parts::PgLoad(unsigned int *data, const unsigned int *data_end)
{
	PartGfx tex{};
	while (data < data_end) {
		unsigned int *buf = data;
		++data;
		if (!memcmp(buf, "PGNM", 4)) {
			tex.name = (char*)data;
			data += 0x20/4;
		} else if (!memcmp(buf, "PGTP", 4)) {
			tex.type = data[0];
			++data;
		} else if (!memcmp(buf, "PGTE", 4)) { //UNI
			//two shorts? Size again?
			++data;
		} else if (!memcmp(buf, "PGT2", 4)) { //UNI
			//data[0] is size + 16?
			//tex.w = data[1];
			//tex.h = data[2];
			data += 3;
		} else if (!memcmp(buf, "DXT5", 4)) { //UNI
			//??
			int size = data[4];
			data += 6;

			char *cdata = (char *)data;
			//TODO: Load DDS at cdata
			tex.data = cdata;
			cdata += size; //Length
			data = (unsigned int *)cdata;
		} else if (!memcmp(buf, "PGTX", 4)) {
			tex.w = data[0];
			tex.h = data[1];
			tex.bpp = data[2];
			tex.data = (char*)(data+3);

			assert(tex.bpp == 32);
			data += (tex.w * tex.h) + 3;
		} else if (!memcmp(buf, "PGED", 4)) {
			textures.push_back(tex);
			break;
		} else {
			char tag[5]{};
			memcpy(tag,buf,4);
			std::cout <<"\tUnknown PG level tag: " << tag <<"\n";
		}
	}
	return data;
}


unsigned int* Parts::PpLoad(unsigned int *data, const unsigned int *data_end)
{
	PatternPart pp{};

	while (data < data_end) {
		unsigned int *buf = data;
		++data;
		if (!memcmp(buf, "PPNM", 4)) {
			pp.name = (char*)data;
			data += 0x20/4;
		} else if (!memcmp(buf, "PPNA", 4)) { //UNI
			//Non null terminated name. Sjis
			unsigned char *cdata = (unsigned char *)data;
			cdata += cdata[0]+1; //Length
			data = (unsigned int *)cdata;
		}  else if (!memcmp(buf, "PPCC", 4)) {
			//two ints. coords
			data += 2;
		} else if (!memcmp(buf, "PPUV", 4)) {
			//Texture coords?

			data += 4;
		} else if (!memcmp(buf, "PPSS", 4)) {
			//two ints
			data += 2;
		} else if (!memcmp(buf, "PPTE", 4)) { //UNI
			//two shorts?
			++data;
		} else if (!memcmp(buf, "PPPA", 4)) { //UNI
			//int
			++data;
		} else if (!memcmp(buf, "PPTP", 4)) {
			//int
			++data;
		} else if (!memcmp(buf, "PPTX", 4)) {
			//int
			++data;
		} else if (!memcmp(buf, "PPJP", 4)) {
			//Optional
			//2 ints
			data += 2;
		} else if (!memcmp(buf, "PPED", 4)) {
			break;
		} else {
			char tag[5]{};
			memcpy(tag,buf,4);
			std::cout <<"\tUnknown PP level tag: " << tag <<"\n";
		}
	}
	return data;
}

unsigned int* Parts::PrLoad(unsigned int *data, const unsigned int *data_end)
{
	while (data < data_end) {
		unsigned int *buf = data;
		++data;
		if (!memcmp(buf, "PRXY", 4)) {
			//two sint32 coords
			data += 2;
		} else if (!memcmp(buf, "PRAL", 4)) { //Opt
			unsigned char *cdata = (unsigned char *)data;
			//1 byte
			++cdata;
			data = (unsigned int *)cdata;
		} else if (!memcmp(buf, "PRRV", 4)) { //Opt
			unsigned char *cdata = (unsigned char *)data;
			//1 byte
			++cdata;
			data = (unsigned int *)cdata;
		} else if (!memcmp(buf, "PRFL", 4)) { //Opt
			unsigned char *cdata = (unsigned char *)data;
			//1 byte
			++cdata;
			data = (unsigned int *)cdata;
		} else if (!memcmp(buf, "PRZM", 4)) {
			//two floats
			data += 2;
		} else if (!memcmp(buf, "PRSP", 4)) {
			//idk
			++data;
		} else if (!memcmp(buf, "PRAN", 4)) {
			//???
			++data;
		} else if (!memcmp(buf, "PRPR", 4)) {
			//int
			++data;
		} else if (!memcmp(buf, "PRID", 4)) {
			//int
			++data;
		} else if (!memcmp(buf, "PRCL", 4)) {
			//Seems optional
			//Color key maybe??
			++data;
		} else if (!memcmp(buf, "PRA3", 4)) { //UNI
			data += 4;
		} else if (!memcmp(buf, "PRED", 4)) {
			break;
		} else {
			char tag[5]{};
			memcpy(tag,buf,4);
			std::cout <<"\tUnknown PR level tag: " << tag <<"\n";
		}
	}
	return data;
}

unsigned int* Parts::P_Load(unsigned int *data, const unsigned int *data_end)
{
	while (data < data_end) {
		unsigned int *buf = data;
		++data;
		if (!memcmp(buf, "PANM", 4)) {
			//A name?
			data += 0x20/4;
		} else if (!memcmp(buf, "PANA", 4)) { //UNI
			//Non null terminated name
			unsigned char *cdata = (unsigned char *)data;
			cdata += cdata[0]+1; //Length
			data = (unsigned int *)cdata;
		} else if (!memcmp(buf, "PRST", 4)) {
			//some id = data[0];
			++data;
			data = PrLoad(data, data_end);
		} else if (!memcmp(buf, "P_ED", 4)) {
			break;
		} else {
			char tag[5]{};
			memcpy(tag,buf,4);
			std::cout <<"\tUnknown P_ level tag: " << tag <<"\n";
		}
	}
	return data;
}


unsigned int* Parts::MainLoad(unsigned int *data, const unsigned int *data_end)
{
	while (data < data_end) {
		unsigned int *buf = data;
		++data;
		
		if (!memcmp(buf, "P_ST", 4)) {
			unsigned int p_id = *data;
			++data;
			data = P_Load(data, data_end);
		} else if (!memcmp(buf, "PPST", 4)) {
			//some id = data[0];
			++data;
			data = PpLoad(data, data_end);
		} else if (!memcmp(buf, "PGST", 4)) {
			//some id = data[0];
			++data;
			data = PgLoad(data, data_end);
		} else if (!memcmp(buf, "VEST", 4)) { //UNI
			int amount = data[0];
			int len = data[1];
			data += amount*len + 2;
			data = VeLoad(data, data_end, amount);
		} else if (!memcmp(buf, "_END", 4)) {
			break;
		} else {
			char tag[5]{};
			memcpy(tag,buf,4);
			std::cout <<"\tUnknown top level tag: " << tag <<"\n";
		}
	}
	return data;
}

bool Parts::Load(const char *name)
{
	char *data;
	unsigned int size;
	if (!ReadInMem(name, data, size))
		return false;

	if (memcmp(data, "PAniDataFile", 12))
	{
		delete[] data;
		return false;
	}

	unsigned int *d = (unsigned int *)(data + 0x20);
	unsigned int *d_end = (unsigned int *)(data + size);
	if (memcmp(d, "_STR", 4)) {
		delete[] data;
		return false;
	}

	delete[] this->data;
	this->data = data;
	textures.clear();

	MainLoad(d+1, d_end);
	return true;
}

ImageData *Parts::GetTexture(unsigned int n)
{
	if(n >= 0 && n < textures.size())
	{
		ImageData *texture = new ImageData{};
		texture->width = textures[n].w;
		texture->height = textures[n].h;
		texture->bgr = true;

		size_t size = texture->width*texture->height*4;
		texture->pixels = new unsigned char[size];
		memcpy(texture->pixels, textures[n].data, size);

		return texture;
	}

	return nullptr;
} 