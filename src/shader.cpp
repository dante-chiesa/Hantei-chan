#include "shader.h"

#include <sstream>
#include <fstream>
#include <string>
#include <windows.h>
#include <glad/glad.h>
#include <iostream>

std::string ReadFile(const char *filePath)
{
    std::string content;
    std::ifstream fileStream(filePath, std::ios::in);

    if(!fileStream.is_open()) {
        return "";
    }

    std::string line = "";
    while(!fileStream.eof()) {
        std::getline(fileStream, line);
        content.append(line + "\n");
    }

	//std::cerr <<"TEST -> \n"<< content << "\n";

    fileStream.close();
    return content;
}

GLuint CreateShader(const char **src, GLenum type)
{
	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, src, nullptr);
	glCompileShader(shader);
	GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if(!success)
	{
		char log[2048];
		glGetShaderInfoLog(shader, 2048, 0, log);
		std::stringstream ss;
		switch(type)
		{
			case GL_VERTEX_SHADER:
				ss << "Vertex ";
				break;
			case GL_FRAGMENT_SHADER:
				ss << "Frament ";
				break;
			default:
				ss << "Other ";
		}
		ss << "Shader error:\n" << log;
		std::cerr << ss.str() << "\n";
	}
	return shader;
}

Shader::Shader(): program(0)
{
	program = glCreateProgram();
	if (program == 0)
		std::cerr << "Shader program creation failed\n";
}
Shader::~Shader()
{
	glDeleteProgram(program);
}
Shader::Shader(const char *vertex_path, const char *fragment_path): Shader()
{
	LoadShader(vertex_path, fragment_path);
}

int Shader::GetLoc(const char* name)
{
	return glGetUniformLocation(program, name);
}

int Shader::GetAttribLoc(const char* name)
{
	return glGetAttribLocation(program, name);
	GLint a;
}

void Shader::BindAttrib(const char* name, int index)
{
	glBindAttribLocation(program, index, name);
}

void Shader::LoadShader(const char *vertex_path, const char *fragment_path, bool sourceString)
{
	std::string vertShaderStr, fragShaderStr;
	const char *vertShaderSrc = "";
	const char *fragShaderSrc = "";

	if(sourceString)
	{
		vertShaderSrc = vertex_path;
		fragShaderSrc = fragment_path;
	}
	else
	{
		
		if(vertex_path != nullptr && fragment_path != nullptr)
		{
			vertShaderStr = ReadFile(vertex_path);
			fragShaderStr = ReadFile(fragment_path);
			vertShaderSrc = vertShaderStr.c_str();
			fragShaderSrc = fragShaderStr.c_str();
		}
	}
	

	GLuint myVertexShader = CreateShader(&vertShaderSrc, GL_VERTEX_SHADER);
	GLuint myFragShader = CreateShader(&fragShaderSrc, GL_FRAGMENT_SHADER);

	std::stringstream ss;

	glAttachShader(program, myVertexShader);
	glAttachShader(program, myFragShader);
	glLinkProgram(program);
	GLint success;
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if(!success)
	{
		char log[2048];
		glGetProgramInfoLog(program, 2048, 0, log);
		ss << "Shader linking error: "<<vertex_path<<" & "<<fragment_path<<":\n" << log <<"\n";
	}
	else
		std::cerr <<"...No errors.\n";

	if(!ss.str().empty())
		std::cerr << ss.str() << "\n";

	glDeleteShader(myVertexShader);
	glDeleteShader(myFragShader);
}

void Shader::Use()
{
	glUseProgram(program);
}