#include "Shader.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

Shader::Shader(const GLchar *vertexShaderPath, const GLchar *fragmentShaderPath)
{
	std::string vertexShader;
	std::string fragmentShader;

	std::ifstream vertexShaderFile;
	std::ifstream fragmentShaderFile;

	vertexShaderFile.exceptions(std::ifstream::badbit);
	fragmentShaderFile.exceptions(std::ifstream::badbit);

	try
	{
		vertexShaderFile.open(vertexShaderPath);
		fragmentShaderFile.open(fragmentShaderPath);

		std::stringstream vertexShaderStream, fragmentShaderStream;
		vertexShaderStream << vertexShaderFile.rdbuf();
		fragmentShaderStream << fragmentShaderFile.rdbuf();

		vertexShaderFile.close();
		fragmentShaderFile.close();

		vertexShader = vertexShaderStream.str();
		fragmentShader = fragmentShaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cerr << "error: fail to read shader files" << std::endl;
	}

	const GLchar *vertexShaderCode = vertexShader.c_str();
	const GLchar *fragmentShaderCode = fragmentShader.c_str();

	GLint success;
	GLchar infoLog[512];

	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderID, 1, &vertexShaderCode, NULL);
	glCompileShader(vertexShaderID);
	glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShaderID, 512, NULL, infoLog);
		std::cout << "glCompileShader VERTEX fail\n" << infoLog << std::endl;
	}

	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderID, 1, &fragmentShaderCode, NULL);
	glCompileShader(fragmentShaderID);
	glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShaderID, 512, NULL, infoLog);
		std::cout << "glCompileShader FRAGMENT fail\n" << infoLog << std::endl;
	}

	this->program = glCreateProgram();
	glAttachShader(this->program, vertexShaderID);
	glAttachShader(this->program, fragmentShaderID);
	glLinkProgram(this->program);
	glGetProgramiv(this->program, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(this->program, 512, NULL, infoLog);
		std::cout << "glLinkProgram fail\n" << infoLog << std::endl;
	}

	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);
}

GLuint Shader::getProgram() const
{
	return program;
}
