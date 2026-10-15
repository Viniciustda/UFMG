// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include <algorithm>
#include <vector>
#include "Game.h"
#include "Actors/Ship.h"
#include "Actors/Asteroid.h"
#include "Components/DrawComponent.h"
#include "Components/RigidBodyComponent.h"
#include "Components/CircleColliderComponent.h"
#include "Random.h"
#include "Components/ParticleSystemComponent.h"

Game::Game()
        :mWindow(nullptr)
        ,mRenderer(nullptr)
        ,mTicksCount(0)
        ,mIsRunning(true)
        ,mIsDebugging(false)
        ,mUpdatingActors(false)
        ,mShip(nullptr)
        ,mCurrentFibIndex(0)
{
    mFibonacciSequence = {1, 2, 3, 5, 8, 13, 21};
}

bool Game::Initialize()
{
    Random::Init();

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return false;
    }

    mWindow = SDL_CreateWindow("TP 2 - Asteroids - Vinicius Trindade", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
    if (!mWindow)
    {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        return false;
    }

    mRenderer = new Renderer(mWindow);
    mRenderer->Initialize(WINDOW_WIDTH, WINDOW_HEIGHT);

    // Init all game actors
    InitializeActors();

    mTicksCount = SDL_GetTicks();

    return true;
}

void Game::InitializeActors()
{
    mShip = new Ship(this, 30.0f, 200.0f, Math::Pi);
    mShip->SetPosition(Vector2(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f));

    mExplosion = new Actor(this);
    std::vector<Vector2> particleVertices;
    particleVertices.emplace_back(Vector2(-2.0f, 0.0f));
    particleVertices.emplace_back(Vector2(2.0f, 0.0f));
    new ParticleSystemComponent(mExplosion, particleVertices, 50, 10, Vector3(1.0f, 0.5f, 0.0f), false);
    mExplosion->SetState(ActorState::Paused);

    SpawnWave();
}

void Game::Explosion(const Vector2& position)
{
    // Move the explosion actor to the correct position
    mExplosion->SetPosition(position);

    // Get its particle system and emit particles
    auto ps = mExplosion->GetComponent<ParticleSystemComponent>();
    if (ps) {
        int numParticles = Random::GetIntRange(20, 30);
        for (int i = 0; i < numParticles; ++i) {
            float speed = Random::GetFloatRange(1000.0f, 5000.0f);
            float lifetime = Random::GetFloatRange(0.5f, 1.5f);
            ps->EmitParticle(lifetime, speed, ParticleDirection::Random);
        }
    }
}

void Game::SpawnWave()
{
    const int numAsteroids = mFibonacciSequence[mCurrentFibIndex];
    const float safeRadius = 250.0f;
    Vector2 shipPos = mShip->GetPosition();

    for (int i = 0; i < numAsteroids; ++i) {
        Vector2 astPos;
        do {
            astPos.x = Random::GetFloatRange(0.0f, WINDOW_WIDTH);
            astPos.y = Random::GetFloatRange(0.0f, WINDOW_HEIGHT);
        } while ((astPos - shipPos).Length() < safeRadius);

        auto* ast = new Asteroid(this, Random::GetFloatRange(40.0f, 60.0f), true, Random::GetIntRange(8, 12), 1500.0f);
        ast->SetPosition(astPos);
    }
}

void Game::RunLoop()
{
    while (mIsRunning)
    {
        ProcessInput();
        UpdateGame();
        GenerateOutput();
    }
}

void Game::ProcessInput()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_QUIT:
                Quit();
                break;
        }
    }

    const Uint8* state = SDL_GetKeyboardState(nullptr);
    for (auto actor : mActors) {
        actor->ProcessInput(state);
    }
}

void Game::UpdateGame()
{
    while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16));

    float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
    if (deltaTime > 0.05f)
    {
        deltaTime = 0.05f;
    }

    mTicksCount = SDL_GetTicks();

    // Update all actors and pending actors
    UpdateActors(deltaTime);

    // Check if all asteroids are destroyed to spawn the next wave
    if (mAsteroids.empty() && mShip->GetState() == ActorState::Active) {
        mCurrentFibIndex++;
        if (mCurrentFibIndex >= mFibonacciSequence.size()) {
            mCurrentFibIndex = 0; // Reset to the beginning
        }
        SpawnWave();
    }
}

void Game::UpdateActors(float deltaTime)
{
    // Update actors
    mUpdatingActors = true;
    for (auto actor : mActors) {
        actor->Update(deltaTime);
    }
    mUpdatingActors = false;

    // Move pending actors to mActors
    for (auto pending : mPendingActors) {
        mActors.emplace_back(pending);
    }
    mPendingActors.clear();

    // Remove dead actors
    std::vector<Actor*> deadActors;
    for (auto actor : mActors) {
        if (actor->GetState() == ActorState::Destroy) {
            deadActors.emplace_back(actor);
        }
    }

    for (auto actor : deadActors) {
        delete actor;
    }
}

void Game::AddAsteroid(Asteroid* ast)
{
    mAsteroids.emplace_back(ast);
}

void Game::RemoveAsteroid(Asteroid* ast)
{
    std::vector<class Asteroid*>::iterator it;
    it = std::find(mAsteroids.begin(), mAsteroids.end(), ast);

    if (it != mAsteroids.end())
    {
        mAsteroids.erase(it);
    }
}

void Game::AddActor(Actor* actor)
{
    if (mUpdatingActors) {
        mPendingActors.emplace_back(actor);
    }
    else {
        mActors.emplace_back(actor);
    }
}

void Game::RemoveActor(Actor* actor)
{
    auto iter = std::find(mPendingActors.begin(), mPendingActors.end(), actor);
    if (iter != mPendingActors.end()) {
        std::iter_swap(iter, mPendingActors.end() - 1);
        mPendingActors.pop_back();
    }

    iter = std::find(mActors.begin(), mActors.end(), actor);
    if (iter != mActors.end()) {
        std::iter_swap(iter, mActors.end() - 1);
        mActors.pop_back();
    }
}

void Game::AddDrawable(class DrawComponent *drawable)
{
    mDrawables.emplace_back(drawable);
    std::sort(mDrawables.begin(), mDrawables.end(), [](DrawComponent* a, DrawComponent* b) {
        return a->GetDrawOrder() < b->GetDrawOrder();
    });
}

void Game::RemoveDrawable(class DrawComponent *drawable)
{
    auto iter = std::find(mDrawables.begin(), mDrawables.end(), drawable);
    if (iter != mDrawables.end()) {
        mDrawables.erase(iter);
    }
}

void Game::GenerateOutput()
{
    // Clear back buffer
    mRenderer->Clear();

    for (auto drawable : mDrawables) {
        drawable->Draw(mRenderer);
    }

    if (mIsDebugging) {
        for (auto actor : mActors) {
            if (actor->GetState() == ActorState::Active) {
                for (auto component : actor->GetComponents()) {
                    component->DebugDraw(mRenderer);
                }
            }
        }
    }

    // Swap front buffer and back buffer
    mRenderer->Present();
}

void Game::Shutdown()
{
    while (!mActors.empty()) {
        delete mActors.back();
    }

    mRenderer->Shutdown();
    delete mRenderer;
    mRenderer = nullptr;

    SDL_DestroyWindow(mWindow);
    SDL_Quit();
}