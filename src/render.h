#ifndef RENDER_H_GUARD
#define RENDER_H_GUARD

#include "cg.h"
#include "parts.h"
#include "texture.h"
#include "shader.h"
#include "vao.h"
#include "hitbox.h"
#include <vector>
#include <unordered_map>
#include <glm/mat4x4.hpp>

class Render
{
private:
	glm::mat4 projection, view;
	
	CG *cg;
	Parts *parts;
	Vao vSprite;
	Vao vGeometry;
	enum{
		LINES = 0,
		BOXES,
		GEO_SIZE
	};
	int geoParts[GEO_SIZE];
	float imageVertex[6*4];
	std::vector<float> clientQuads;
	int quadsToDraw;

	int lProjectionS, lProjectionT;
	int lAlphaS, lAddColorT;
	Shader sSimple;
	Shader sTextured;
	Texture texture;
	float colorRgba[4];

	int curImageId;
	bool curPat;
	

	void AdjustImageQuad(int x, int y, int w, int h);
	void SetModelView(glm::mat4&& view);
	void SetMatrix(int location);
	void SetBlendingMode();
	
	float iScale;

public:
	bool drawLines = true;
	bool filter;
	int x, offsetX;
	int y, offsetY;
	float scale;
	float scaleX, scaleY;
	float rotX, rotY, rotZ;
	int highLightN = -1;
	
	Render(CG* cg, Parts* parts);
	void Draw();
	void UpdateProj(float w, float h);

	void GenerateHitboxVertices(const BoxList &hitboxes);
	void SwitchImage(int id, bool pat = false);
	void DontDraw();
	void SetImageColor(float *rgbaArr);
	void SetScale(float scale);

	enum blendType{
		normal,
		additive,
		substractive
	};

	blendType blendingMode;
};

#endif /* RENDER_H_GUARD */
