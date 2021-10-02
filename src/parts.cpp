#include "parts.h"
#include "misc.h"

#include <algorithm>
#include <iostream>
#include <cassert>

#include <glm/gtc/matrix_transform.hpp>

struct counts_{
	int p_name = 0;
	int ppname = 0;
	int pgname = 0;
} counts;

Parts::Parts() : partVertices(Vao::F2F2, GL_STATIC_DRAW)
{

}

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

//Graphic data
unsigned int* Parts::PgLoad(unsigned int *data, const unsigned int *data_end, int id)
{
	PartGfx tex{};
	while (data < data_end) {
		unsigned int *buf = data;
		++data;
		if (!memcmp(buf, "PGNM", 4)) {
			tex.name = (char*)data;
			std::cout << counts.pgname <<" PG: "<<(char*)data << "\n";
			data += 0x20/4;
			counts.pgname +=1;
		} else if (!memcmp(buf, "PGTP", 4)) {
			tex.type = data[0];
			std::cout <<"\tGraphic type"<<tex.type <<"\n";
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
			std::cout <<"\t"<<tex.w <<"x" <<tex.h <<" "<<tex.bpp<<"bpp\n";

			assert(tex.bpp == 32);
			data += (tex.w * tex.h) + 3;
		} else if (!memcmp(buf, "PGED", 4)) {
			gfxMeta.insert({id,tex});
			break;
		} else {
			char tag[5]{};
			memcpy(tag,buf,4);
			std::cout <<"\tUnknown PG level tag: " << tag <<"\n";
		}
	}
	return data;
}

//More like box cut-outs
unsigned int* Parts::PpLoad(unsigned int *data, const unsigned int *data_end, int id)
{
	CutOut pp{};

	while (data < data_end) {
		unsigned int *buf = data;
		++data;
		if (!memcmp(buf, "PPNM", 4)) {
			pp.name = (char*)data;
			std::cout << counts.ppname <<" PP: "<<(char*)data << "\n";
			data += 0x20/4;
			counts.ppname +=1;
		} else if (!memcmp(buf, "PPNA", 4)) { //UNI
			//Non null terminated name. Sjis
			unsigned char *cdata = (unsigned char *)data;
			cdata += cdata[0]+1; //Length
			data = (unsigned int *)cdata;
		}  else if (!memcmp(buf, "PPCC", 4)) {
			//XY. Transform coordinates seem inverted because they're vertex coordinates.
			//This fransform is applied earlier.
			memcpy(pp.xy, data, sizeof(int)*2);
			data += 2;
		} else if (!memcmp(buf, "PPUV", 4)) {
			//Texture coords
			//LEFT, TOP, W, H
			memcpy(pp.uv, data, sizeof(int)*4);
			data += 4;
		} else if (!memcmp(buf, "PPSS", 4)) {
			//W and H of coordinates. +Y is down as usual.
			memcpy(pp.wh, data, sizeof(int)*2);
			data += 2;
		} else if (!memcmp(buf, "PPTE", 4)) { //UNI
			//two shorts?
			++data;
		} else if (!memcmp(buf, "PPPA", 4)) { //UNI
			++data;
		} else if (!memcmp(buf, "PPTP", 4)) {
			//Texture reference id. Default is 0.
			std::cout << "\t"<<"PPTP " << *data << "\t-";
			pp.texture = *data;
			++data;
		} else if (!memcmp(buf, "PPTX", 4)) {
			//No idea. Doesn't seem to do anything.
			std::cout << "\t"<<"PPTX " << *data << "\n";
			++data;
		} else if (!memcmp(buf, "PPJP", 4)) {
			//Some XY offset for when the part "grabs" the player
			//Doesn't affect rendering so we don't care about it.
			data += 2;
		} else if (!memcmp(buf, "PPED", 4)) {
			cutOuts.insert({id,pp});
			break;

		} else {
			char tag[5]{};
			memcpy(tag,buf,4);
			std::cout <<"\tUnknown PP level tag: " << tag <<"\n";
		}
	}
	return data;
}

//Part properties
unsigned int* Parts::PrLoad(unsigned int *data, const unsigned int *data_end, int groupId, int propId)
{
	PartProperty pr{};
	while (data < data_end) {
		unsigned int *buf = data;
		++data;
		if (!memcmp(buf, "PRXY", 4)) {
			pr.x = ((int*)data)[0];
			pr.y = ((int*)data)[1];
			data += 2;
		} else if (!memcmp(buf, "PRAL", 4)) { //Opt
			unsigned char *cdata = (unsigned char *)data;
			pr.additive = *cdata;
			++cdata;
			data = (unsigned int *)cdata;
		} else if (!memcmp(buf, "PRRV", 4)) { //Opt
			unsigned char *cdata = (unsigned char *)data;
			pr.reverse = *cdata;
			++cdata;
			data = (unsigned int *)cdata;
		} else if (!memcmp(buf, "PRFL", 4)) { //Opt
			unsigned char *cdata = (unsigned char *)data;
			pr.filter = *cdata;
			++cdata;
			data = (unsigned int *)cdata;
		} else if (!memcmp(buf, "PRZM", 4)) {
			pr.scaleX = *((float*)data);
			pr.scaleY = *((float*)data+1);
			data += 2;
		} else if (!memcmp(buf, "PRSP", 4)) {
			//Add color. bgra
			memcpy(pr.addColor, data, sizeof(char)*4);
			++data;
		} else if (!memcmp(buf, "PRAN", 4)) {
			//Float, rotation, clockwise. 1.f = 360
			pr.rotation = (*(float*)data);
			++data;
		} else if (!memcmp(buf, "PRPR", 4)) {
			//Priority. Higher value means draw first / lower on the stack.
			pr.priority = *data;
			pr.priority += propId/1000.f; //AAAAAAA
			++data;
		} else if (!memcmp(buf, "PRID", 4)) {
			//Part id. Which thing to render
			pr.ppId = *data;
			++data;
		} else if (!memcmp(buf, "PRCL", 4)) {
			//Color key
			memcpy(pr.bgra, data, sizeof(char)*4);
			++data;
		} else if (!memcmp(buf, "PRA3", 4)) { //UNI
			data += 4;
		} else if (!memcmp(buf, "PRED", 4)) {
			groups[groupId].push_back(pr);
			break;
		} else {
			char tag[5]{};
			memcpy(tag,buf,4);
			std::cout <<"\tUnknown PR level tag: " << tag <<"\n";
		}
	}
	return data;
}

//Contains many parts
unsigned int* Parts::P_Load(unsigned int *data, const unsigned int *data_end, int id)
{
	const char* name = "";
	bool hasData = false;
	while (data < data_end) {
		unsigned int *buf = data;
		++data;
		if (!memcmp(buf, "PANM", 4)) {
			//A name?
			name = (char*)data;
			data += 0x20/4;
			counts.p_name +=1;
		} else if (!memcmp(buf, "PANA", 4)) { //UNI
			//Non null terminated name
			unsigned char *cdata = (unsigned char *)data;
			cdata += cdata[0]+1; //Length
			data = (unsigned int *)cdata;
		} else if (!memcmp(buf, "PRST", 4)) {
			groups.insert({id,{}});
			int propId = data[0];
			hasData = true;
			++data;
			data = PrLoad(data, data_end, id, propId);
		} else if (!memcmp(buf, "P_ED", 4)) {
			break;
		} else {
			char tag[5]{};
			memcpy(tag,buf,4);
			std::cout <<"\tUnknown P_ level tag: " << tag <<"\n";
		}
	}
	if(hasData)
		std::cout << id <<" _P: "<< name << "\n";
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
			data = P_Load(data, data_end, p_id);
		} else if (!memcmp(buf, "PPST", 4)) {
			int id = data[0];
			++data;
			data = PpLoad(data, data_end, id);
		} else if (!memcmp(buf, "PGST", 4)) {
			int id = data[0];
			++data;
			data = PgLoad(data, data_end, id);
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
	counts = {};
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
	for(auto &tex :textures)
		delete tex;
	textures.clear();
	cutOuts.clear();
	gfxMeta.clear();
	groups.clear();
	partVertices.Clear();
	MainLoad(d+1, d_end);
	std::cout << std::endl;

	//Load textures
	for(auto &gfxKv : gfxMeta)
	{
		auto &gfx = gfxKv.second;
		textures.push_back(new Texture);
		textures.back()->LoadDirect(gfx.data, gfx.w, gfx.h);
		gfx.textureIndex=textures.back()->id;
	}

	//Load vertex data in vao.
	std::vector<float> vertexData;
	vertexData.reserve(128*4);
	for(auto &partKv : cutOuts)
	{
		auto &part = partKv.second;
		constexpr int tX[] = {0,1,1, 1,0,0};
		constexpr int tY[] = {0,0,1, 1,1,0};
		constexpr int tXI[] = {1,0,0,0,1,1};
		constexpr int tYI[] = {1,1,0,0,0,1,};
		struct{
			float x,y,s,t;
		}point[6];

		if(gfxMeta.count(part.texture) == 0)
		{
			std::cout << "There's no graphic id "<< part.texture<<" requested by part id "<<partKv.first<<"\n";
			continue;
		}
		float width = 256;//gfxMeta[part.texture].w/2.f;
		float height = 256;//gfxMeta[part.texture].h/2.f;
		for(int i = 0; i < 6; i++)
		{
			point[i].x = -part.xy[0] + part.wh[0]*tX[i];
			point[i].y = -part.xy[1] + part.wh[1]*tY[i];
			point[i].s = float(part.uv[0] + part.uv[2]*tX[i])/width;
			point[i].t = float(part.uv[1] + part.uv[3]*tY[i])/height;
		}
		auto size = vertexData.size();
		vertexData.resize(size+6*4);
		memcpy(&vertexData[size], point, sizeof(point));
	}
	int index = 0;
	for(auto &cutOut : cutOuts)
	{
		if(gfxMeta.count(cutOut.second.texture) == 0)
			continue;
		cutOut.second.vaoIndex = partVertices.Prepare(6*4*sizeof(float), &vertexData[index]);
		index+=6*4;
	}

	//Sort parts in group by priority;
	for(auto &group : groups)
	{
		auto &v = group.second;
		std::stable_sort(v.rbegin(),v.rend(),[](const PartProperty& a, const PartProperty& b){
			return a.priority < b.priority;
		});
	}

	partVertices.Load();
	return true;
}

void Parts::Draw(int pattern, const glm::mat4 &projection,
	std::function<void(glm::mat4)> setMatrix,
	std::function<void(float,float,float)> setAddColor, float color[4])
{
	static float f = 0.1;
	curTexId = -1;
	if(groups.count(pattern))
	{
		partVertices.Bind();
		for(auto &part : groups[pattern])
		{
			if(cutOuts.count(part.ppId) == 0)
				continue;

			constexpr float tau = glm::pi<float>()*2.f;
			glm::mat4 view = glm::mat4(1.f);
			
			if(part.reverse)
			{
				//f+=0.5;
				auto cutout = cutOuts[part.ppId];
				//view = glm::translate(view, glm::vec3(-xy[0],part.y,0.f));
				
				view = glm::translate(view, glm::vec3(part.x,part.y,0.f));
 				view = glm::rotate(view, (part.rotation)*tau, glm::vec3(0.0, 0.f, 1.f));
				//view = glm::translate(view, glm::vec3(cutout.wh[0]/2,0.f,0.f));
				auto dif = f;
				//view = glm::translate(view, glm::vec3(dif,0.f,0.f));
				view = glm::scale(view, glm::vec3(-1.f, 1.f, 1.f));
				view = glm::translate(view, glm::vec3(-cutout.wh[0]+cutout.xy[0]*2,0.f,0.f));
				//view = glm::rotate(view, f*tau, glm::vec3(0.0, 0.f, 1.f));
				//view = glm::translate(view, glm::vec3(-part.x*sin(part.rotation*tau), 1.f, 1.f));
				f+=0.25;
			}
			else{
				view = glm::translate(view, glm::vec3(part.x,part.y,0.f));
				view = glm::rotate(view, part.rotation*tau, glm::vec3(0.0, 0.f, 1.f));
			}
			
			view = glm::scale(view, glm::vec3(part.scaleX, part.scaleY, 1.f));
			
			//setMatrix(glm::scale(projection, glm::vec3(0.75f,0.75f,1.f))*view);
			setMatrix(projection*view);
			if(screenShot)
				glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA, GL_ONE);
			else if(part.additive)
				glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			else
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			
			float newColor[4];
			memcpy(newColor, color, sizeof(float)*4);
			newColor[0] *= part.bgra[2]/255.f;
			newColor[1] *= part.bgra[1]/255.f;
			newColor[2] *= part.bgra[0]/255.f;
			newColor[3] *= part.bgra[3]/255.f;
			glVertexAttrib4fv(2, newColor);
			setAddColor(part.addColor[2]/255.f, part.addColor[1]/255.f,part.addColor[0]/255.f);
			DrawPart(part.ppId);
		}
	}
}

void Parts::DrawPart(int i)
{
	if(cutOuts.count(i) > 0 && gfxMeta.count(cutOuts[i].texture))
	{
		if(curTexId != gfxMeta[cutOuts[i].texture].textureIndex)
		{
			curTexId = gfxMeta[cutOuts[i].texture].textureIndex;
			glBindTexture(GL_TEXTURE_2D, curTexId);
		}
		partVertices.Draw(cutOuts[i].vaoIndex);
	}
}

ImageData *Parts::GetTexture(unsigned int n)
{
	if(n >= 0 && n < gfxMeta.size())
	{
		ImageData *texture = new ImageData{};
		texture->width = gfxMeta[n].w;
		texture->height = gfxMeta[n].h;
		texture->bgr = true;

		size_t size = texture->width*texture->height*4;
		texture->pixels = new unsigned char[size];
		memcpy(texture->pixels, gfxMeta[n].data, size);

		return texture;
	}

	return nullptr;
} 