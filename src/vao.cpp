#include "vao.h"
#include <glad/glad.h>
#include <windows.h>
#include <sstream>

Vao::Vao(AttribType _type, unsigned int _usage):
type(_type), usage(_usage), stride(0), totalSize(0), vaoId(0), vboId(0),
quadCount(nullptr), quadIndexes(nullptr)
{
	switch(type)
	{
	case F2F2:
		stride = 4 * sizeof(float);
		break;
	case F3F3:
		stride = 6 * sizeof(float);
		break;
	}
}

Vao::~Vao()
{
	glDeleteBuffers(1, &vboId);
	delete[] quadIndexes;
}

void Vao::Clear()
{
	glDeleteBuffers(1, &vboId);
	delete[] quadIndexes;
	vaoId = 0;
	vboId = 0;
	totalSize = 0;
	quadCount = nullptr;
	quadIndexes = nullptr;
	dataPointers.clear();
}

int Vao::Prepare(size_t size, void *ptr)
{
	if(int alignment = size % stride)
	{
		std::stringstream ss;
		ss << "Size "<<size<<" is not a multiple of the stride "<<stride;
		MessageBoxA(nullptr, ss.str().c_str(), __FUNCTION__, MB_ICONWARNING);
		size -= alignment;
	}

	dataPointers.push_back(memPtr{
		(uint8_t*) ptr,
		size,
		totalSize/stride
	});
	totalSize += size;
	return dataPointers.size() - 1;
}

void Vao::Draw(int which, size_t count, int mode)
{
	if(count == 0)
		count = dataPointers[which].size/stride;
	glDrawArrays(mode, dataPointers[which].location, count);
}

void Vao::InitQuads(int which)
{
	if(!quadIndexes)
	{
		quadSize = (dataPointers[which].size/stride)/4;
		quadIndexes = new GLint[quadSize*2];
		quadCount = quadIndexes+quadSize;
		for(int i = 0; i < quadSize; i++)
		{
			quadIndexes[i] = dataPointers[which].location + i*4;
			quadCount[i] = 4;
		}
	}
}

void Vao::DrawQuads(int mode, int numberOf)
{
	if(numberOf<0)
		numberOf = quadSize;
	if(quadIndexes)
		glMultiDrawArrays(mode, quadIndexes, quadCount, numberOf);
}

void Vao::UpdateBuffer(int which, void *data, size_t count)
{
	if(count == 0)
		count = dataPointers[which].size;

	glBindBuffer(GL_ARRAY_BUFFER, vboId);
	glBufferSubData(GL_ARRAY_BUFFER, dataPointers[which].location*stride, count, data);

}

void Vao::Bind()
{
	//Since we now have to keep the state ourselves.
	glBindBuffer(GL_ARRAY_BUFFER, vboId);
	switch(type)
	{
	case F3F3:
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, nullptr);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float)*3));
		glEnableVertexAttribArray(1);
		break;
	case F2F2:
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, nullptr);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float)*2));
		glEnableVertexAttribArray(1);
		break;
	}
}

void Vao::Load()
{
	glGenBuffers(1, &vboId);
	glBindBuffer(GL_ARRAY_BUFFER, vboId);
	glBufferData(GL_ARRAY_BUFFER, totalSize, nullptr, usage);

	uint8_t *data = (uint8_t*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	size_t where = 0;
	for(auto &subData : dataPointers)
	{
		if(subData.ptr)
			memcpy(data+where, subData.ptr, subData.size);
		where += subData.size;
	}
	glUnmapBuffer(GL_ARRAY_BUFFER);

	switch(type)
	{
	case F3F3:
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, nullptr);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float)*3));
		glEnableVertexAttribArray(1);
		break;
	case F2F2:
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, nullptr);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float)*2));
		glEnableVertexAttribArray(1);
		break;
	}

}