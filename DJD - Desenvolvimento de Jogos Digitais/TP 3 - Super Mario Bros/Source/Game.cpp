// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include <algorithm>
#include <vector>
#include <map>
#include <fstream>
#include "CSV.h"
#include "Game.h"
#include "Components/Drawing/DrawComponent.h"
#include "Components/Physics/RigidBodyComponent.h"
#include "Components/Drawing/AnimatorComponent.h"
#include "Random.h"
#include "Actors/Actor.h"
#include "Actors/Block.h"
#include "Actors/Goomba.h"
#include "Actors/Spawner.h"
#include "Actors/Mario.h"
#include "Actors/Mushroom.h"
#include "Actors/Coin.h"

Game::Game()
        :mWindow(nullptr)
        ,mRenderer(nullptr)
        ,mTicksCount(0)
        ,mIsRunning(true)
        ,mIsDebugging(false)
        ,mUpdatingActors(false)
        ,mCameraPos(Vector2::Zero)
        ,mMario(nullptr)
        ,mLevelData(nullptr)
        ,mIsMarioDead(false)
        ,mCoinCount(0)
{

}

bool Game::Initialize()
{
    Random::Init();

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return false;
    }

    mWindow = SDL_CreateWindow("TP3: Super Mario Bros", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
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
    // Calculate the total width of the level in pixels
    const float levelWidth = LEVEL_WIDTH * TILE_SIZE;

    // Create a single background actor
    auto* background = new Actor(this);
    // Position its center in the middle of the entire level
    background->SetPosition(Vector2(levelWidth / 2.0f, WINDOW_HEIGHT / 2.0f));
    // Create the component with the full level width, stretching the texture
    new AnimatorComponent(background, "../Assets/Sprites/Background.png", "", levelWidth, WINDOW_HEIGHT, 10);
    
    // Load level data and build level
    mLevelData = LoadLevel("../Assets/Levels/Level1-2/level1-2.csv", LEVEL_WIDTH, LEVEL_HEIGHT);
    BuildLevel(mLevelData, LEVEL_WIDTH, LEVEL_HEIGHT);
}

int **Game::LoadLevel(const std::string& fileName, int width, int height)
{
    // Allocate the 2D array for the level data
    int** levelData = new int*[height];
    for (int i = 0; i < height; ++i) {
        levelData[i] = new int[width];
    }

    // Open the CSV file
    std::ifstream file(fileName);
    if (!file.is_open()) {
        SDL_Log("Failed to load level file: %s", fileName.c_str());
        return nullptr;
    }

    std::string line;
    int row = 0;
    // Read each line of the file
    while (std::getline(file, line) && row < height) {
        // Split the line into integers
        std::vector<int> tiles = CSVHelper::Split(line, ',');
        if (tiles.size() != width) {
            SDL_Log("Invalid line in level file: %s", fileName.c_str());
            // You might want to handle this error more gracefully
        }

        // Store the tile IDs in the 2D array
        for (int col = 0; col < width; ++col) {
            levelData[row][col] = tiles[col];
        }
        row++;
    }

    return levelData;
}

void Game::BuildLevel(int** levelData, int width, int height)
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            int tileID = levelData[i][j];
            if (tileID != -1)
            {
                Vector2 pos(j * TILE_SIZE + TILE_SIZE / 2.0f, i * TILE_SIZE + TILE_SIZE / 2.0f);
                Block* block = nullptr;

                switch (tileID)
                {
                    case 0: // Floor
                        block = new Block(this, "BlockA.png");
                        break;
                    case 1: // Question Mark
                        block = new Block(this, "BlockC.png");
                        break;
                    case 2: // Pipe Top Left
                        block = new Block(this, "BlockF.png");
                        break;
                    case 3: // Coin
                    {
                        auto* coin = new Coin(this, false);
                        coin->SetPosition(pos);
                        break;
                    }
                    case 4: // Brick
                        block = new Block(this, "BlockB.png");
                        break;
                    case 6: // Pipe Body Right
                        block = new Block(this, "BlockI.png");
                        break;
                    case 8: // Solid Block
                        block = new Block(this, "BlockD.png");
                        break;
                    case 9: // Pipe Body Left
                        block = new Block(this, "BlockH.png");
                        break;
                    case 12: // Pipe Top Right
                        block = new Block(this, "BlockG.png");
                        break;
                    case 13: // Question Mark (with Mushroom)
                        block = new Block(this, "BlockC.png", true);
                        break;
                    case 16: // Mario
                    {
                        mMario = new Mario(this);
                        mMario->SetPosition(pos);
                        break;
                    }
                    case 10: // Goomba Spawner
                    {
                        auto* spawner = new Spawner(this, 400.0f);
                        spawner->SetPosition(pos);
                        break;
                    }
                }

                if (block) {
                    block->SetPosition(pos);
                }
            }
        }
    }
}

void Game::RunLoop()
{
    while (mIsRunning)
    {
        // Calculate delta time in seconds
        float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
        if (deltaTime > 0.05f)
        {
            deltaTime = 0.05f;
        }

        mTicksCount = SDL_GetTicks();

        ProcessInput();
        UpdateGame(deltaTime);
        GenerateOutput();

        // Sleep to maintain frame rate
        int sleepTime = (1000 / FPS) - (SDL_GetTicks() - mTicksCount);
        if (sleepTime > 0)
        {
            SDL_Delay(sleepTime);
        }
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
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    Quit();
                }
                if (mIsMarioDead && event.key.keysym.sym == SDLK_RETURN) {
                    Restart();
                }
                break;
        }
    }

    const Uint8* state = SDL_GetKeyboardState(nullptr);

    for (auto actor : mActors)
    {
        actor->ProcessInput(state);
    }
}

void Game::UpdateGame(float deltaTime)
{
    // Update all actors and pending actors
    UpdateActors(deltaTime);

    // Update camera position
    UpdateCamera();
}

void Game::UpdateActors(float deltaTime)
{
    mUpdatingActors = true;
    for (auto actor : mActors)
    {
        actor->Update(deltaTime);
    }
    mUpdatingActors = false;

    for (auto pending : mPendingActors)
    {
        mActors.emplace_back(pending);
    }
    mPendingActors.clear();

    std::vector<Actor*> deadActors;
    for (auto actor : mActors)
    {
        if (actor->GetState() == ActorState::Destroy)
        {
            deadActors.emplace_back(actor);
        }
    }

    for (auto actor : deadActors)
    {
        delete actor;
    }
}

void Game::UpdateCamera()
{
    if (mMario) {
        // Center camera on Mario
        float targetX = mMario->GetPosition().x - (WINDOW_WIDTH / 2.0f);

        // Clamp camera position to the left edge
        mCameraPos.x = std::max(mCameraPos.x, targetX);

        // Clamp camera position to the right edge
        float maxCameraX = (LEVEL_WIDTH * TILE_SIZE) - WINDOW_WIDTH;
        mCameraPos.x = std::min(mCameraPos.x, maxCameraX);
    }
}

void Game::AddActor(Actor* actor)
{
    if (mUpdatingActors)
    {
        mPendingActors.emplace_back(actor);
    }
    else
    {
        mActors.emplace_back(actor);
    }
}

void Game::RemoveActor(Actor* actor)
{
    auto iter = std::find(mPendingActors.begin(), mPendingActors.end(), actor);
    if (iter != mPendingActors.end())
    {
        // Swap to end of vector and pop off (avoid erase copies)
        std::iter_swap(iter, mPendingActors.end() - 1);
        mPendingActors.pop_back();
    }

    iter = std::find(mActors.begin(), mActors.end(), actor);
    if (iter != mActors.end())
    {
        // Swap to end of vector and pop off (avoid erase copies)
        std::iter_swap(iter, mActors.end() - 1);
        mActors.pop_back();
    }
}

void Game::AddDrawable(class DrawComponent *drawable)
{
    mDrawables.emplace_back(drawable);

    std::sort(mDrawables.begin(), mDrawables.end(),[](DrawComponent* a, DrawComponent* b) {
        return a->GetDrawOrder() < b->GetDrawOrder();
    });
}

void Game::RemoveDrawable(class DrawComponent *drawable)
{
    auto iter = std::find(mDrawables.begin(), mDrawables.end(), drawable);
    mDrawables.erase(iter);
}

void Game::AddCollider(class AABBColliderComponent* collider)
{
    mColliders.emplace_back(collider);
}

void Game::RemoveCollider(AABBColliderComponent* collider)
{
    auto iter = std::find(mColliders.begin(), mColliders.end(), collider);
    mColliders.erase(iter);
}

void Game::GenerateOutput()
{
    // Clear back buffer
    mRenderer->Clear();

    for (auto drawable : mDrawables)
    {
        drawable->Draw(mRenderer);

        if(mIsDebugging)
        {
           // Call draw for actor components
              for (auto comp : drawable->GetOwner()->GetComponents())
              {
                comp->DebugDraw(mRenderer);
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

    // Delete level data
    if (mLevelData) {
        for (int i = 0; i < LEVEL_HEIGHT; ++i) {
            delete[] mLevelData[i];
        }
        delete[] mLevelData;
        mLevelData = nullptr;
    }

    mRenderer->Shutdown();
    delete mRenderer;
    mRenderer = nullptr;

    SDL_DestroyWindow(mWindow);
    SDL_Quit();
}

void Game::Restart()
{
    // Clean up all existing actors
    while (!mActors.empty()) {
        delete mActors.back();
    }
    mPendingActors.clear();

    // Ensure component lists are empty
    mDrawables.clear();
    mColliders.clear();

    // Reset game state
    mIsMarioDead = false;
    mCameraPos = Vector2::Zero;
    mMario = nullptr;
    mCoinCount = 0;

    // Reinitialize level
    InitializeActors();

    // Reset delta time calculation
    mTicksCount = SDL_GetTicks();
}