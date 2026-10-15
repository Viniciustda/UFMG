//
//  Game.cpp
//  Game-mac
//
//  Created by Sanjay Madhav on 5/31/17.
//  Copyright © 2017 Sanjay Madhav. All rights reserved.
//

#include "Game.h"
#include <algorithm>
#include <fstream>
#include "Renderer/Renderer.h"
#include "Random.h"
#include "Actors/Paddle.h"
#include "Actors/Ball.h"

Game::Game()
:mRenderer(nullptr)
,mWindow(nullptr)
,mPaddle1(nullptr)
,mPaddle2(nullptr)
,mBall(nullptr)
,mIsRunning(true)
,mTicksCount(0)
,mDash(nullptr)
{
}

bool Game::Initialize()
{
	Random::Init();

	// Inicializa o subsistema de vídeo da SDL
	if (SDL_Init (SDL_INIT_VIDEO) < 0) {
		SDL_Log("Nao foi possivel inicializar o subsistema de video da SDL: %s", SDL_GetError());
		return false;
	}

	// Cria a janela
	mWindow = SDL_CreateWindow (
		"TP 1 - Pong - Vinicius Trindade",	// título
		SDL_WINDOWPOS_CENTERED,				// posição X da janela
		SDL_WINDOWPOS_CENTERED,				// posição Y da janela
		WINDOW_WIDTH,		                // largura
		WINDOW_HEIGHT,				        // altura
		SDL_WINDOW_OPENGL					// flags
	);

	if (mWindow == nullptr)	{
		SDL_Log("Nao foi possivel criar a janela: %s", SDL_GetError());
		return false;
	}

	// Instancia o renderer
	mRenderer = new Renderer(mWindow);

	// Inicializa o renderer
	if (!mRenderer->Initialize(WINDOW_WIDTH, WINDOW_HEIGHT)) {
		SDL_Log("Nao foi possivel inicializar o renderer");
		return false;
	}

	// Carrega os dados do jogo 
	LoadData();

	// Define o tempo inicial em milissegundos
	mTicksCount = SDL_GetTicks();

	return true;
}

void Game::RunLoop()
{
	while (mIsRunning) {
		ProcessInput();
		Update();
		GenerateOutput();
	}
}

void Game::ProcessInput()
{
	SDL_Event event;
	// Processa todos os eventos na fila
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT) {
			mIsRunning = false;
		}
	}

	// Verifica estado do teclado
	const Uint8* state = SDL_GetKeyboardState(nullptr);
	if (state[SDL_SCANCODE_ESCAPE]) {
		mIsRunning = false;
	}

	// Repassa o estado do teclado para a raquete
    if (mPaddle1) {
        mPaddle1->ProcessInput(state);
    }
	if (mPaddle2) {
        mPaddle2->ProcessInput(state);
    }
}

void Game::Update()
{
	// Espera que passe pelo menos 16ms desde o último frame (~60 FPS)
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16)) {}

	// Calcula o delta time (tempo entre o frame atual e o anterior)
	Uint32 currentTicks = SDL_GetTicks();
	float deltaTime = (currentTicks - mTicksCount) / 1000.0f; // ms -> segundos

	// Limita deltaTime a no máximo 0.05s (para estabilidade em debug)
	if (deltaTime > 0.05f) {
		deltaTime = 0.05f;
	}

	// Atualiza o mTicksCount para o frame atual
	mTicksCount = currentTicks;

	if (mPaddle1) {
		mPaddle1->Update(deltaTime);
	}
	if (mPaddle2) {
        mPaddle2->Update(deltaTime);
    }
	if (mBall) {
		mBall->Update(deltaTime);
	}
}

void Game::GenerateOutput()
{
	// Limpa a tela 
	mRenderer->Clear();

	// Desenha a linha tracejada no meio
    Vector3 dashColor(1.0f, 1.0f, 1.0f);
    for (int i = 0; i < 20; ++i)
    {
        Matrix4 dashModel = Matrix4::CreateTranslation(
            Vector3(WINDOW_WIDTH / 2.0f, 25.0f + i * 40.0f, 0.0f)
        );
        mRenderer->Draw(dashModel, dashColor, mDash);
    }

	if (mPaddle1) {
		mPaddle1->Draw(mRenderer);
	}
	if (mPaddle2) {
        mPaddle2->Draw(mRenderer);
    }
	if (mBall) {
		mBall->Draw(mRenderer);
	}

	// Desenha objetos do jogo no back buffer
	mRenderer->Present();
}

void Game::LoadData()
{
	// Cria os traços da quadra
    const float dashVerts[] = {
        -2.5f, -12.5f,
        2.5f, -12.5f,
        2.5f, 12.5f,
        -2.5f, 12.5f 
    };
    const unsigned int dashIndices[] = {
        0, 1, 2,
        0, 2, 3
    };
    mDash = new VertexArray(dashVerts, 4, dashIndices, 6);

    // Cria a raquete
    int paddleW = 20;
	int paddleH = 100;
	mPaddle1 = new Paddle(this, paddleW, paddleH, PlayerIdentifier::Player1);
	mPaddle2 = new Paddle(this, paddleW, paddleH, PlayerIdentifier::Player2);

    // Cria a bolinha 
    int ballSize = 15;
	mBall = new Ball(this, ballSize, ballSize);
}

void Game::UnloadData()
{
	// Deleta os atores
	if (mDash) {
        delete mDash;
        mDash = nullptr;
    }
	if (mPaddle1) {
		delete mPaddle1;
		mPaddle1 = NULL;
	}
	if (mPaddle2) {
        delete mPaddle2;
        mPaddle2 = NULL;
    }
	if (mBall) {
		delete mBall;
		mBall = NULL;
	}
}

void Game::Shutdown()
{
	UnloadData();

	// Destrói o renderer
	if (mRenderer) {
		mRenderer->Shutdown();
		delete mRenderer;
		mRenderer = NULL;
	}

	SDL_Quit();
}

void Game::ResetRound()
{
	// Reposiciona e reseta a bola
    mBall->Reset();

    // Reposiciona a raquete 1
    Vector2 paddle1Pos;
    paddle1Pos.x = 30.0f;
    paddle1Pos.y = static_cast<float>(WINDOW_HEIGHT) / 2.0f;
    mPaddle1->SetPosition(paddle1Pos);

    // Reposiciona a raquete 2
    Vector2 paddle2Pos;
    paddle2Pos.x = static_cast<float>(WINDOW_WIDTH) - 30.0f;
    paddle2Pos.y = static_cast<float>(WINDOW_HEIGHT) / 2.0f;
    mPaddle2->SetPosition(paddle2Pos);
}