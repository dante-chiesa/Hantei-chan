#ifndef PARTS_H_GUARD
#define PARTS_H_GUARD

#include "image.h"
#include "texture.h"
#include "vao.h"
#include <vector>
#include <unordered_map>
#include <functional>
#include <glm/mat4x4.hpp>

class Parts{
private:
	char *data = nullptr;

	//Box cut-out
	struct CutOut 
	{
		char *name;
		
		int uv[4];
		int xy[2];
		int wh[2];
		int texture = 0;
		//int type;
		int jump;
		int vaoIndex;
	};

	struct PartGfx{
		char *name;
		char *data;
		int w,h;
		int bpp;
		int type;
		int textureIndex;
	};

	struct PartProperty{
		float priority;
		float rotation;
		float x;
		float y;
		float scaleX = 1.f;
		float scaleY = 1.f;
		int ppId;
		bool additive;
		bool filter;
		bool reverse;
		unsigned char bgra[4] = {255,255,255,255};
		unsigned char addColor[4] = {0,0,0,0};
	};

	//Has id and a list of parts
	
	std::unordered_map<int,std::vector<PartProperty>> groups;
	std::unordered_map<int, CutOut> cutOuts;
	std::unordered_map<int, PartGfx> gfxMeta;
	std::vector<Texture*> textures;
	Vao partVertices;
	int curTexId;

	unsigned int *MainLoad(unsigned int *data, const unsigned int *data_end);
	unsigned int *P_Load(unsigned int *data, const unsigned int *data_end, int id);
	unsigned int *PrLoad(unsigned int *data, const unsigned int *data_end, int groupId, int propId);
	unsigned int *PpLoad(unsigned int *data, const unsigned int *data_end, int id);
	unsigned int *PgLoad(unsigned int *data, const unsigned int *data_end, int id);
	unsigned int *VeLoad(unsigned int *data, const unsigned int *data_end, int amount);
	void DrawPart(int id);
	
public:
	Parts();
	bool Load(const char *name);

	ImageData *GetTexture(unsigned int n);
	void Draw(int pattern, const glm::mat4 &projection, std::function<void(glm::mat4)> setMatrix,
		std::function<void(float,float,float)> setAddColor, float color[4]);
	
};

#endif /* PARTS_H_GUARD */
