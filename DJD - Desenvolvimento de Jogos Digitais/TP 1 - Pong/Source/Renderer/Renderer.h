#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <SDL.h>
#include "../Math.h"
#include "VertexArray.h"

class Renderer
{
public:
	Renderer(SDL_Window* window);
	~Renderer();

	bool Initialize(float width, float height);
	void Shutdown();

    void Clear();
    void Draw(const Matrix4 &modelMatrix, const Vector3& color, VertexArray* vertexArray);

	void Present();

    // Getters
    class Shader* GetBaseShader() const { return mBaseShader; }
    //class VertexArray* GetSpriteVerts() const { return mRectVertices; }

private:
	bool LoadShaders();
	//void CreateRectVertices();

	// Game
	class Game* mGame;

	// Sprite shader
	class Shader* mBaseShader;

    // Vertex array
	//class VertexArray* mRectVertices;

	// Window
	SDL_Window* mWindow;

	// OpenGL context
	SDL_GLContext mContext;

    // Ortho projection for 2D shaders
    Matrix4 mOrthoProjection;
};
