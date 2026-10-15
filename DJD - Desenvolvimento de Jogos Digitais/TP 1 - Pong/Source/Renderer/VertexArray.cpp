#include "VertexArray.h"
#include <GL/glew.h>

VertexArray::VertexArray(const float* verts, unsigned int numVerts, const unsigned int* indices,
						 unsigned int numIndices)
: mNumVerts(numVerts)
, mNumIndices(numIndices)
, mVertexBuffer(0)
, mIndexBuffer(0)
, mVertexArray(0)
{
	// VAO
	glGenVertexArrays(1, &mVertexArray);
	glBindVertexArray(mVertexArray);

	// VBO
	glGenBuffers(1, &mVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, numVerts * 2 * sizeof(float), verts, GL_STATIC_DRAW);

	// Especificar no VAO como o arranjo de vértices deve ser lido pelo vertex shader
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0,                  // location = 0 no vertex shader
		2,                  // 2 componentes por vértice (x, y)
		GL_FLOAT,           // tipo
		GL_FALSE,           // não normalizar
		2 * sizeof(float),  // stride: tamanho de cada vértice
		(void*)0            // offset inicial
	);

	// EBO
	glGenBuffers(1, &mIndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(unsigned int), indices, GL_STATIC_DRAW);
}

VertexArray::~VertexArray()
{
	if (mVertexArray != 0) {
		glDeleteVertexArrays(1, &mVertexArray);
		mVertexArray = 0;
	}

	if (mVertexBuffer != 0) {
		glDeleteBuffers(1, &mVertexBuffer);
		mVertexBuffer = 0;
	}

	if (mIndexBuffer != 0) {
		glDeleteBuffers(1, &mIndexBuffer);
		mIndexBuffer = 0;
	}
}

void VertexArray::SetActive() const
{
	// Ativa o VAO
	glBindVertexArray(mVertexArray);

	// Ativa o EBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
}
