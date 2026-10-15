#pragma once
#include "SDL.h"
#include <unordered_map>
#include <string>
#include <vector>
#include "Math.h"

class Game
{
public:
	Game();
	bool Initialize();
    void RunLoop();
	void Shutdown();
    void Quit() { mIsRunning = false; }
	void ResetRound();

    class Paddle* GetPaddle1() const { return mPaddle1; }
	class Paddle* GetPaddle2() const { return mPaddle2; }

	class Renderer* GetRenderer() { return mRenderer; }

	static const int WINDOW_WIDTH = 1024;
    static const int WINDOW_HEIGHT = 768;

private:
	void ProcessInput();
	void Update();
	void GenerateOutput();

    void LoadData();
	void UnloadData();

    // Window
    SDL_Window* mWindow;

    // Renderer
	class Renderer* mRenderer;

    // Game-specific
    class Paddle* mPaddle1; // Player's paddle
	class Paddle* mPaddle2; // Player 2's paddle
    class Ball* mBall; // Moving ball
	class VertexArray* mDash;

	// Game state

	Uint32 mTicksCount;
	bool mIsRunning;
};
