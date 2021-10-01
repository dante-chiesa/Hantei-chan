#include "texture.h"
#include "cg.h"
#include <glad/glad.h>

#include <fstream>
#include <iostream>
#include <string>
#include <cassert>

int pal = 0; //temporary

Texture::Texture() : id(0), isLoaded(false), isApplied(false){}; //initializes as 0
Texture::Texture(Texture&& texture)
{
	image = std::move(texture.image);
	id = texture.id;
	isLoaded = texture.isLoaded;
	isApplied = texture.isApplied;
	//filename = std::move(texture.filename);

	texture.isApplied = false;
	texture.isLoaded = false;
}

Texture::~Texture()
{
	if(isApplied) //This gets triggered by vector.resize() when the class instance is NOT a pointer.
		Unapply();
	if(isLoaded)
		Unload();
}

void Texture::Load(ImageData *data)
{
	const char *palette = nullptr;
	image.reset(data);
	isLoaded = true;
}

void Texture::LoadDirect(char *data, int w, int h, bool bgr)
{
	isApplied = true;
	glGenTextures(1, &id);

	glBindTexture(GL_TEXTURE_2D, id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	GLenum extType = GL_RGBA;
	if(bgr)
		extType = GL_BGRA;
	GLenum intType = GL_RGBA8;
	
	glTexImage2D(GL_TEXTURE_2D, 0, intType, w, h, 0, extType, GL_UNSIGNED_BYTE, data);
}

void Texture::Apply(bool repeat, bool linearFilter)
{
	isApplied = true;
	glGenTextures(1, &id);

	glBindTexture(GL_TEXTURE_2D, id);
	if(repeat)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	}
	if(linearFilter)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}

	GLenum extType = GL_RGBA;
	if(image->bgr)
		extType = GL_BGRA;
	GLenum intType = GL_RGBA8;
	
	glTexImage2D(GL_TEXTURE_2D, 0, intType, image->width, image->height, 0, extType, GL_UNSIGNED_BYTE, image->pixels);
}

void Texture::Unapply()
{
	isApplied = false;
	glDeleteTextures(1, &id);
}
void Texture::Unload()
{
	isLoaded = false;
	image.reset();
}
