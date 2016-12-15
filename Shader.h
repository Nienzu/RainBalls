#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>

class Shader
{
public:

	Shader(const GLchar *vertexShaderPath, const GLchar *fragmentShaderPath);
	GLuint getProgram() const;

private:

	GLuint program;
};

#endif
