#include <GL/glew.h>
#include "Renderer.h"
#include "Shader.h"
#include "../Game.h"
#include "../Actors/Paddle.h"

Renderer::Renderer(SDL_Window *window)
: mBaseShader(nullptr)
//, mRectVertices(nullptr)
, mWindow(window)
, mContext(nullptr)
{
}

Renderer::~Renderer()
{
}

bool Renderer::Initialize(float width, float height)
{
    // Specify version 3.3 (core profile)
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // Enable double buffering
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    // Force OpenGL to use hardware acceleration
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

    // Turn on vsync
    SDL_GL_SetSwapInterval(1);

	// Cria contexto OpenGL
	mContext = SDL_GL_CreateContext(mWindow);
	if (!mContext) {
		SDL_Log("Failed to create OpenGL context: %s", SDL_GetError());
		return false;
	}

	// Inicializa GLEW
	if (glewInit() != GLEW_OK) {
		SDL_Log("Failed to initialize GLEW");
		return false;
	}

	// Carrega shaders
	if (!LoadShaders()) {
		SDL_Log("Failed to load shaders");
		return false;
	}

	// Ativar o shader para poder setar uniforms
    mBaseShader->SetActive();

	// Cria vértices do retângulo
	//CreateRectVertices();

	// Ativa VAO/EBO do retângulo
	//mRectVertices->SetActive();

	// Cor de fundo (preto)
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Create orthografic projection matrix
    mOrthoProjection = Matrix4::CreateOrtho(0.0f, width, height, 0.0f, -1.0f, 1.0f);
	mBaseShader->SetMatrixUniform("uOrthoProj", mOrthoProjection);

    return true;
}

void Renderer::Shutdown()
{
	if (mBaseShader) {
		mBaseShader->Unload();
		delete mBaseShader;
		mBaseShader = nullptr;
	}

	/*if (mRectVertices) {
		delete mRectVertices;
		mRectVertices = nullptr;
	}*/

	if (mContext) {
		SDL_GL_DeleteContext(mContext);
		mContext = nullptr;
	}

	if (mWindow) {
		SDL_DestroyWindow(mWindow);
		mWindow = nullptr;
	}
}

void Renderer::Clear()
{
	glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::Draw(const Matrix4 &modelMatrix, const Vector3& color, VertexArray* vertexArray)
{

	// Ativa o VertexArray do ator específico
    vertexArray->SetActive();

	// Atualiza a matriz de modelo no shader
	mBaseShader->SetMatrixUniform("uWorldTransform", modelMatrix);

	// Atualiza a cor no shader
	mBaseShader->SetVectorUniform("uColor", color);

	// Desenha usando os índices (triângulos)
	glDrawElements(
		GL_TRIANGLES,					// tipo de primitiva
		vertexArray->GetNumIndices(), 	// quantidade de índices
		GL_UNSIGNED_INT,				// tipo dos índices
		0								// offset (0 -> começo do EBO)
	);
}

void Renderer::Present()
{
	SDL_GL_SwapWindow(mWindow);
}

bool Renderer::LoadShaders()
{
	// Cria o objeto Shader
	mBaseShader = new Shader();

	// Carrega os shaders usando o caminho base (sem extensão)
	if (!mBaseShader->Load("../../Shaders/Base")) {
		SDL_Log("Failed to load shaders.");
		return false;
	}
	
	return true;
}

/*void Renderer::CreateRectVertices()
{
	// Vértices do retângulo (x, y) em sentido horário
	const float verts[] = {
		-0.5f, 0.5f,   // 0: esquerda-superior
		0.5f,  0.5f,   // 1: direita-superior
		0.5f, -0.5f,   // 2: direita-inferior
		-0.5f, -0.5f    // 3: esquerda-inferior
	};

	// Índices para dois triângulos
	const unsigned int indices[] = {
		0, 1, 2,   // Triângulo 1
		0, 2, 3    // Triângulo 2
	};

	// Cria VertexArray e guarda em mRectVertices
	mRectVertices = new VertexArray(verts, 4, indices, 6);
}*/