#ifndef VAO_H_GUARD
#define VAO_H_GUARD
#include <vector>
#include <cstdint>
#include <glad/glad.h>

//Not really a VAO since this is OGL 2.1
class Vao
{
public:
	enum AttribType
	{
		F2F2, //Textures
		F3F3 //Color only
	};

private:
	AttribType type;
	unsigned int usage;
	int stride;

	struct memPtr
	{
		uint8_t *ptr;
		size_t size;
		size_t location;
	};

	size_t totalSize;
	std::vector<memPtr> dataPointers;
	unsigned int vaoId;
	unsigned int vboId;

	GLint *quadIndexes;
	GLint *quadCount;
	GLsizei quadSize;

public:
	Vao(AttribType type, unsigned int usage);
	~Vao();

	//Returns index of object that can be drawn.
	int Prepare(size_t size, void *ptr);
	void Draw(int which, size_t count = 0, int mode = GL_TRIANGLES);

	void InitQuads(int which);
	void DrawQuads(int mode = GL_LINE_LOOP, int numberOf =-1);
	void UpdateBuffer(int which, void *data, size_t count = 0);
	void Bind();
	void Load();
	void Clear();
	
};

#endif /* VAO_H_GUARD */
