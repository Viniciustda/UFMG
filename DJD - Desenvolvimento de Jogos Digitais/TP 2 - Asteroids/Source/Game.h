// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include <SDL.h>
#include <vector>
#include "Actors/Ship.h"
#include "Renderer/Renderer.h"

class Game
{
public:
    Game();

    bool Initialize();
    void RunLoop();
    void Shutdown();
    void Quit() { mIsRunning = false; }

    // Actor functions
    void InitializeActors();
    void UpdateActors(float deltaTime);
    void AddActor(class Actor* actor);
    void RemoveActor(class Actor* actor);

    // Renderer
    class Renderer* GetRenderer() { return mRenderer; }

    static const int WINDOW_WIDTH = 1024;
    static const int WINDOW_HEIGHT = 768;

    // Draw functions
    void AddDrawable(class DrawComponent* drawable);
    void RemoveDrawable(class DrawComponent* drawable);

    std::vector<class DrawComponent*>& GetDrawables() { return mDrawables; }

    class Ship *GetShip() const {return mShip; }

    // Game-specific (add/remove asteroid)
    void AddAsteroid(class Asteroid* ast);
    void RemoveAsteroid(class Asteroid* ast);
    std::vector<class Asteroid*>& GetAsteroids() { return mAsteroids; }

    void Explosion(const Vector2& position);

private:
    void ProcessInput();
    void UpdateGame();
    void GenerateOutput();
    void SpawnWave();

    // All the actors in the game
    std::vector<class Actor*> mActors;
    std::vector<class Actor*> mPendingActors;

    // All the draw components
    std::vector<class DrawComponent*> mDrawables;

    // SDL stuff
    SDL_Window* mWindow;
    class Renderer* mRenderer;

    // Track elapsed time since game start
    Uint32 mTicksCount;

    // Track if we're updating actors right now
    bool mIsRunning;
    bool mIsDebugging;
    bool mUpdatingActors;

    // Game-specific
    class Ship* mShip;
    class Actor* mExplosion;
    std::vector<class Asteroid*> mAsteroids;
    std::vector<int> mFibonacciSequence;
    int mCurrentFibIndex;
};
