#include "render.h"
#include "main.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>

//Error
#include <windows.h>
#include <glad/glad.h>
#include <sstream>
#include <iostream>

#include "hitbox.h"
#include "misc.h"

constexpr int maxBoxes = 33;

const char* simpleSrcVert = R"(
#version 120
attribute vec3 Position;
attribute vec3 Color;

varying vec4 Frag_Color;

uniform mat4 ProjMtx;
uniform float Alpha;

void main()
{
	Frag_Color = vec4(Color, Alpha);
	gl_Position = ProjMtx * vec4(Position, 1);
}
)";

const char* simpleSrcFrag = R"(
#version 120

varying vec4 Frag_Color;

void main()
{
	gl_FragColor = Frag_Color;
}
)";

const char* texturedSrcVert = R"(
#version 120
attribute vec2 Position;
attribute vec2 UV;
attribute vec4 Color;

varying vec2 Frag_UV;
varying vec4 Frag_Color;

uniform mat4 ProjMtx;

void main()
{
	Frag_UV = UV;
	Frag_Color = Color;
	gl_Position = ProjMtx * vec4(Position.xy, 0, 1);
}
)";

const char* texturedSrcFrag = R"(
#version 120
uniform sampler2D Texture;

varying vec2 Frag_UV;
varying vec4 Frag_Color;

uniform vec3 AddColor;

void main()
{
	vec4 col = texture2D(Texture, Frag_UV.st);
	col.rgb += AddColor;
	gl_FragColor = col * Frag_Color;
}
)";

Render::Render(CG* cg, Parts* parts):
cg(cg),
parts(parts),
filter(false),
vSprite(Vao::F2F2, GL_DYNAMIC_DRAW),
vGeometry(Vao::F3F3, GL_STREAM_DRAW),
imageVertex{
	256, 256, 	0, 0,
	512, 256,  	1, 0, 
	512, 512,  	1, 1, 

	512, 512, 	1, 1,
	256, 512,  	0, 1,
	256, 256,  	0, 0,
},
colorRgba{1,1,1,1},
curImageId(-1),
quadsToDraw(0),
x(0), offsetX(0),
y(0), offsetY(0),
rotX(0), rotY(0), rotZ(0),
blendingMode(normal)
{
	std::cerr<<"Simple\n";
	sSimple.BindAttrib("Position", 0);
	sSimple.BindAttrib("Color", 1);
	//sSimple.LoadShader("shd/simple.vert", "shd/simple.frag");
	sSimple.LoadShader(simpleSrcVert, simpleSrcFrag, true);

	sSimple.Use();

	std::cerr<<"Textured\n";
	sTextured.BindAttrib("Position", 0);
	sTextured.BindAttrib("UV", 1);
	sTextured.BindAttrib("Color", 2);
	//sTextured.LoadShader("shd/textured.vert", "shd/textured.frag");
	sTextured.LoadShader(texturedSrcVert, texturedSrcFrag, true);
	
	lAlphaS = sSimple.GetLoc("Alpha");
	lProjectionS = sSimple.GetLoc("ProjMtx");
	lProjectionT = sTextured.GetLoc("ProjMtx");
	lAddColorT = sTextured.GetLoc("AddColor");

	vSprite.Prepare(sizeof(imageVertex), imageVertex);
	vSprite.Load();

	float lines[]
	{
		-10000, 0, -1,	1,1,1,
		10000, 0, -1,	1,1,1,
		0, 10000, -1,	1,1,1,
		0, -10000, -1,	1,1,1,
	};
	

	geoParts[LINES] = vGeometry.Prepare(sizeof(lines), lines);
	geoParts[BOXES] = vGeometry.Prepare(sizeof(float)*6*4*maxBoxes, nullptr);
	vGeometry.Load();
	vGeometry.InitQuads(geoParts[BOXES]);

	UpdateProj(clientRect.x, clientRect.y);

	glViewport(0, 0, clientRect.x, clientRect.y);
	glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
}

void Render::SetScale(float newScale)
{
	iScale = newScale;
	scale = iScale;
	if(curPat)
		scale *= 0.5f;
}

void Render::Draw()
{
	static unsigned int lastError = 0;
	if(int err = glGetError() && lastError != err)
	{
		lastError = err;
		std::stringstream ss;
		ss << "GL Error: 0x" << std::hex << err << "\n";
		std::cerr << ss.str()<<"\n";
		//assert(0);
		//MessageBoxA(nullptr, ss.str().c_str(), "GL Error", MB_ICONSTOP);
		//PostQuitMessage(1);
	}

	

	//Lines
	glm::mat4 view = glm::mat4(1.f);
	view = glm::scale(view, glm::vec3(scale, scale, 1.f));
	view = glm::translate(view, glm::vec3(x,y,0.f));
	SetModelView(std::move(view));
	SetMatrix(lProjectionS);

	if(drawLines)
	{
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glUniform1f(lAlphaS, 0.25f);
		vGeometry.Bind();
		vGeometry.Draw(geoParts[LINES], 0, GL_LINES);
	}
	//Sprite
	constexpr float tau = glm::pi<float>()*2.f;
	view = glm::mat4(1.f);
	view = glm::scale(view, glm::vec3(scale, scale, 1.f));
	view = glm::translate(view, glm::vec3(x,y,0.f));
	
	view = glm::scale(view, glm::vec3(scaleX,scaleY,0));
	view = glm::rotate(view, rotZ*tau, glm::vec3(0.0, 0.f, 1.f));
	view = glm::rotate(view, rotY*tau, glm::vec3(0.0, 1.f, 0.f));
	auto partView = view = glm::rotate(view, rotX*tau, glm::vec3(1.0, 0.f, 0.f));
	view = glm::translate(view, glm::vec3(-128+offsetX,-224+offsetY,0.f));
	partView = glm::translate(partView, glm::vec3(offsetX,offsetY,0.f));
	sTextured.Use();
	if(texture.isApplied)
	{
		glUniform3f(lAddColorT, 0.f,0.f,0.f);
		SetModelView(std::move(view));
		SetMatrix(lProjectionT);
		glBindTexture(GL_TEXTURE_2D, texture.id);
		SetBlendingMode();
		glDisableVertexAttribArray(2);
		glVertexAttrib4fv(2, colorRgba);
		vSprite.Bind();
		vSprite.Draw(0);
	}
	else if(curPat)
	{
		glDisable(GL_DEPTH_TEST);
		glDisableVertexAttribArray(2);
		parts->Draw(curImageId, partView, [this](glm::mat4 m){			
				SetModelView(std::move(m));
				SetMatrix(lProjectionT);
			},
			[this](float r, float g, float b){
				glUniform3f(lAddColorT,r,g,b);
			},
			colorRgba
		);
		glEnable(GL_DEPTH_TEST);
	}
	//Reset state
	glBlendEquation(GL_FUNC_ADD);
	sSimple.Use();
	vGeometry.Bind();
	if(screenShot)
	{
		glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA, GL_ONE);
		glUniform1f(lAlphaS, 1.0f);
		vGeometry.DrawQuads(GL_LINE_LOOP, quadsToDraw);
		glUniform1f(lAlphaS, 0.5f);
		vGeometry.DrawQuads(GL_TRIANGLE_FAN, quadsToDraw);
	}
	else
	{
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glUniform1f(lAlphaS, 0.6f);
		vGeometry.DrawQuads(GL_LINE_LOOP, quadsToDraw);
		glUniform1f(lAlphaS, 0.3f);
		vGeometry.DrawQuads(GL_TRIANGLE_FAN, quadsToDraw);
	}
}

void Render::SetModelView(glm::mat4&& view_)
{
	view = std::move(view_);
}

void Render::SetMatrix(int lProjection)
{
	glUniformMatrix4fv(lProjection, 1, GL_FALSE, glm::value_ptr(projection*view));
}

void Render::UpdateProj(float w, float h)
{
	projection = glm::ortho<float>(0, w, h, 0, -1024.f, 1024.f);
}

void Render::SwitchImage(int id, bool pat)
{
	if(id != curImageId || curPat != pat)
	{
		curImageId = id;
		curPat = pat;
		if(texture.isApplied)
			texture.Unapply();

		scale = iScale;
	//if(curPat)
		
		if(pat)
		{
			scale *= 0.5f;
			return;
		}

		ImageData *image = nullptr;
		image = cg->draw_texture(id, false, false);

		if(!image)
		{
			return;
		}

		texture.Load(image); //Deletes the resource when done using it.
		texture.Apply(false, filter);
		
		AdjustImageQuad(texture.image->offsetX, texture.image->offsetY, texture.image->width, texture.image->height);
		vSprite.UpdateBuffer(0, imageVertex);
		texture.Unload();
	}
}

void Render::AdjustImageQuad(int x, int y, int w, int h)
{
	w+=x;
	h+=y;

	imageVertex[0] = imageVertex[16] = imageVertex[20] = x;
	imageVertex[4] = imageVertex[8] = imageVertex[12] = w;

	imageVertex[1] = imageVertex[5] = imageVertex[21] = y;
	imageVertex[9] = imageVertex[13] = imageVertex[17] = h;
}

void Render::GenerateHitboxVertices(const BoxList &hitboxes)
{
	int size = hitboxes.size();
	if(size <= 0)
	{
		quadsToDraw = 0;
		return;
	}

	const float *color;
	//red, green, blue, z order
	constexpr float collisionColor[] 	{1, 1, 1, 1};
	constexpr float greenColor[] 		{0.2, 1, 0.2, 2};
	constexpr float shieldColor[] 		{0, 0, 1, 3}; //Not only for shield
	constexpr float clashColor[]		{1, 1, 0, 4};
	constexpr float projectileColor[] 	{0, 1, 1, 5}; //飛び道具
	constexpr float purple[] 			{0.5, 0, 1, 6}; //特別
	constexpr float redColor[] 			{1, 0.2, 0.2, 7};
	constexpr float hiLightColor[]		{1, 0.5, 1, 10};

	constexpr int tX[] = {0,1,1,0};
	constexpr int tY[] = {0,0,1,1};

	int floats = size*4*6; //4 Vertices with 6 attributes.
	if(clientQuads.size() < floats)
		clientQuads.resize(floats);
	
	int dataI = 0;
	for(const auto &boxPair : hitboxes)
	{
		int i = boxPair.first;
		const Hitbox& hitbox = boxPair.second;

		if (highLightN == i)
			color = hiLightColor;
		else if(i==0)
			color = collisionColor;
		else if (i >= 1 && i <= 8)
			color = greenColor;
		else if(i >=9 && i <= 10)
			color = shieldColor;
		else if(i == 11)
			color = clashColor;
		else if(i == 12)
			color = projectileColor;
		else if(i>12 && i<=24)
			color = purple;
		else
			color = redColor;

		
		for(int j = 0; j < 4*6; j+=6)
		{
			//X, Y, Z, R, G, B
			clientQuads[dataI+j+0] = hitbox.xy[0] + (hitbox.xy[2]-hitbox.xy[0])*tX[j/6];
			clientQuads[dataI+j+1] = hitbox.xy[1] + (hitbox.xy[3]-hitbox.xy[1])*tY[j/6];
			clientQuads[dataI+j+2] = color[3]+1000.f;
			clientQuads[dataI+j+3] = color[0];
			clientQuads[dataI+j+4] = color[1];
			clientQuads[dataI+j+5] = color[2];
		}
		dataI += 4*6;
	}
	quadsToDraw = size;
	vGeometry.UpdateBuffer(geoParts[BOXES], clientQuads.data(), dataI*sizeof(float));
}

void Render::DontDraw()
{
	quadsToDraw = 0;
}

void Render::SetImageColor(float *rgba)
{
	if(rgba)
	{
		for(int i = 0; i < 4; i++)
			colorRgba[i] = rgba[i];
	}
	else
	{
		for(int i = 0; i < 4; i++)
			colorRgba[i] = 1.f;
	}
}

void Render::SetBlendingMode()
{
	switch (blendingMode)
	{
	default:
	case normal:
		//glBlendEquation(GL_FUNC_ADD);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		break;
	case additive:
		//glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		break;
	case substractive:
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		glBlendEquation(GL_FUNC_REVERSE_SUBTRACT);
		break;
	}
}