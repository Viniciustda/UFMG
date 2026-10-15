#include <SDL.h>
#include "Shader.h"
#include <fstream>
#include <sstream>

Shader::Shader()
: mVertexShader(0)
, mFragShader(0)
, mShaderProgram(0)
{
}

Shader::~Shader()
{
}

bool Shader::Load(const std::string& name)
{
	// Tenta compilar o shader de vértice
	if (!CompileShader(name + ".vert", GL_VERTEX_SHADER, mVertexShader)) return false;

	// Tenta compilar o shader de fragmento
	if (!CompileShader(name + ".frag", GL_FRAGMENT_SHADER, mFragShader)) return false;

	// Cria um programa na GPU
	mShaderProgram = glCreateProgram();

	// Anexa os shaders compilados
	glAttachShader(mShaderProgram, mVertexShader);
	glAttachShader(mShaderProgram, mFragShader);

	// Faz o link do programa
	glLinkProgram(mShaderProgram);

	// Verify that the program linked successfully
	return IsValidProgram();
}

void Shader::Unload()
{
	if (mShaderProgram != 0) {
		glDeleteProgram(mShaderProgram);
		mShaderProgram = 0;
	}

	if (mVertexShader != 0) {
		glDeleteShader(mVertexShader);
		mVertexShader = 0;
	}

	if (mFragShader != 0) {
		glDeleteShader(mFragShader);
		mFragShader = 0;
	}
}

void Shader::SetActive() const
{
	glUseProgram(mShaderProgram);
}

void Shader::SetMatrixUniform(const char* name, const Matrix4& matrix) const
{
	// Pega a localização do uniform dentro do shader
	GLint loc = glGetUniformLocation(mShaderProgram, name);
	if (loc == -1) return; // -1 significa que não encontrou

	// Envia a matriz 4x4 para a GPU
	glUniformMatrix4fv(loc, 1, GL_FALSE, matrix.GetAsFloatPtr());
}

void Shader::SetVectorUniform(const char* name, const Vector3& vector) const
{
    GLuint loc = glGetUniformLocation(mShaderProgram, name);
    glUniform3fv(loc, 1, &vector.x);
}

bool Shader::CompileShader(const std::string& fileName, GLenum shaderType, GLuint& outShader)
{
	// Open file
	std::ifstream shaderFile(fileName);
	if (shaderFile.is_open())
	{
		// Read all of the text into a string
		std::stringstream sstream;
		sstream << shaderFile.rdbuf();
		std::string contents = sstream.str();
		const char* contentsChar = contents.c_str();

		// Create a shader of the specified type
		outShader = glCreateShader(shaderType);

        // Set the source characters and try to compile
		glShaderSource(outShader, 1, &(contentsChar), nullptr);
		glCompileShader(outShader);

		if (!IsCompiled(outShader))
		{
			SDL_Log("Failed to compile shader %s", fileName.c_str());
			return false;
		}
	}
	else
	{
		SDL_Log("Shader file not found: %s", fileName.c_str());
		return false;
	}

	return true;
}

bool Shader::IsCompiled(GLuint shader)
{
	GLint status = 0;

    // Query the compile status
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

	if (status != GL_TRUE)
	{
		char buffer[512];
		memset(buffer, 0, 512);
		glGetShaderInfoLog(shader, 511, nullptr, buffer);
		SDL_Log("GLSL Compile Failed:\n%s", buffer);
		return false;
	}

	return true;
}

bool Shader::IsValidProgram() const
{

	GLint status = 0;
	// Query the link status
	glGetProgramiv(mShaderProgram, GL_LINK_STATUS, &status);
	if (status != GL_TRUE)
	{
		char buffer[512];
		memset(buffer, 0, 512);
		glGetProgramInfoLog(mShaderProgram, 511, nullptr, buffer);
		SDL_Log("GLSL Link Status:\n%s", buffer);
		return false;
	}

	return true;
}
