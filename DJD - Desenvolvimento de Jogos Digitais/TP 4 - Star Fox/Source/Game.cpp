// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include <algorithm>
#include <vector>
#include <fstream>
#include "Game.h"
#include "Random.h"
#include "Actors/Actor.h"
#include "Actors/Ship.h"
#include "Actors/BlockObstacle.h"
#include "Actors/Block.h"
#include "Camera.h"
#include "UI/Screens/HUD.h"
#include "UI/Screens/MainMenu.h"

Game::Game()
        :mWindow(nullptr)
        ,mRenderer(nullptr)
        ,mTicksCount(0)
        ,mIsRunning(true)
        ,mIsDebugging(false)
        ,mUpdatingActors(false)
        ,mIsPaused(false)
        ,mShip(nullptr)
        ,mCamera(nullptr)
        ,mAudio(nullptr)
        ,mHUD(nullptr)
        ,mNextBlock(0)
        ,mNextObstacle(0)
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

    // Init SDL Image
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags))
    {
        SDL_Log("Unable to initialize SDL_image: %s", IMG_GetError());
        return false;
    }

    // Initialize SDL_ttf
    if (TTF_Init() != 0)
    {
        SDL_Log("Failed to initialize SDL_ttf");
        return false;
    }

    // Initialize SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1)
    {
        SDL_Log("Failed to initialize SDL_mixer");
        return false;
    }

    mWindow = SDL_CreateWindow("TP4: Star Fox", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
    if (!mWindow)
    {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        return false;
    }

    mRenderer = new Renderer(mWindow);
    mRenderer->Initialize(WINDOW_WIDTH, WINDOW_HEIGHT);

    mAudio = new AudioSystem();

    SetScene(GameScene::MainMenu);
    
    mMusicHandle = mAudio->PlaySound("Music.ogg", true);

    mTicksCount = SDL_GetTicks();

    return true;
}

void Game::UnloadScene()
{
    // Use state so we can call this from withing an a actor update
    for(auto *actor : mActors) {
        actor->SetState(ActorState::Destroy);
    }

    // Delete UI screens
    for (auto ui : mUIStack) {
        delete ui;
    }
    mUIStack.clear();

    // Delete obstacle patterns
    for (auto pattern : mObstaclePatterns) {
        for (auto obstacle : pattern) {
            delete obstacle;
        }
    }
    mObstaclePatterns.clear();

    // Stop all sounds
    if (mAudio) {
        mAudio->StopAllSounds(mMusicHandle);
    }

    // Reset pointers
    mShip = nullptr;
    mCamera = nullptr;
    mHUD = nullptr;
}

void Game::SetScene(GameScene nextScene)
{
    UnloadScene();

    switch (nextScene) {
        case GameScene::MainMenu:
            // Create the main menu UI
            new MainMenu(this, "../Assets/Fonts/Arial.ttf");
            break;
        case GameScene::Level1:
            // Create player ship and camera for the level
            mShip = new Ship(this);
            mCamera = new Camera(this, Vector3(-300.0f, 0.0f, 0.0f), Vector3(20.0f, 0.0f, 0.0f),
                                 Vector3(0.0f, 0.0f, 1.0f), 70.0f, 10.0f, 10000.0f);

            // Create and configure the HUD
            mHUD = new HUD(this, "../Assets/Fonts/Arial.ttf");
            mHUD->SetHealth(mShip->GetHealth());
            mHUD->SetScore(0);

            // Build tunnel walls: reset mNextBlock and spawn WALL groups up to TUNNEL_DEPTH
            mNextBlock = 0;
            for (int i = 0; i < TUNNEL_DEPTH; ++i) {
                SpawnWalls();
            }

            // Load obstacle patterns
            LoadObstaclePatterns("../Assets/Blocks/", OBSTACLE_PATTERN_SIZE);

            // Spawn initial obstacles
            mNextObstacle = 0;
            for (int i = 0; i < OBSTACLES_WAVES_IN_SCREEN; ++i) {
                SpawnObstacles();
            }
            break;
    }
}

void Game::SetPaused(bool paused)
{
    mIsPaused = paused;
    if (mShip) {
        // Pause/Unpause the ship actor
        mShip->SetState(paused ? ActorState::Paused : ActorState::Active);

        // Pause/Unpause the ship's looping sounds (background music is not affected)
        if (mAudio) {
            if (paused) {
                mAudio->PauseAllSounds(mMusicHandle);
            }
            else {
                mAudio->ResumeAllSounds(mMusicHandle);
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

void Game::LoadObstaclePatterns(const std::string& dirName, const int nBlockPatterns)
{
    for (int n = 0; n < nBlockPatterns; n++)
    {
        // Load pattern file
        std::string fileName = dirName + std::to_string(n + 1) + ".txt";
        std::ifstream file(fileName);

        if (!file.is_open())
        {
            SDL_Log("Failed to open block pattern file: %s", fileName.c_str());
            continue;
        }

        std::string line;

        // Create vector to store pattern
        std::vector<BlockObstacleItem *> pattern;

        int i = 0;
        while (std::getline(file, line))
        {
            int j = 0;
            std::vector<int> row;
            for (char c : line)
            {
                if (c == 'A')
                {
                    // Create BlockObstacleItem with texture type 3 and coordinates i,j
                    auto* blockPattern = new BlockObstacleItem({3, i, j});
                    pattern.emplace_back(blockPattern);
                }
                else if (c == 'B')
                {
                    // Create BlockObstacleItem with texture type 4 and coordinates i,j
                    auto* blockPattern = new BlockObstacleItem({4, i, j});
                    pattern.emplace_back(blockPattern);
                }

                j++;
            }
            i++;
        }

        // Add pattern to map
        mObstaclePatterns.emplace_back(pattern);

        file.close();
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
                // Handle key press for UI screens
                if (!mUIStack.empty()) {
                    mUIStack.back()->HandleKeyPress(event.key.keysym.sym);
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

void Game::SpawnObstacles()
{
    // Determine which pattern to use
    int patternIndex;
    if (mNextObstacle < mObstaclePatterns.size()) {
        patternIndex = mNextObstacle;
    }
    else {
        // After all patterns have been used, pick one at random
        patternIndex = Random::GetIntRange(0, static_cast<int>(mObstaclePatterns.size()) - 1);
    }

    const auto& pattern = mObstaclePatterns[patternIndex];

    // Calculate the base x-position for this wave of obstacles
    float xPos = static_cast<float>(TUNNEL_DEPTH * WALL_SCALE) +
                 static_cast<float>(mNextObstacle * OBSTACLE_PATTERN_GAP);

    // Instantiate all obstacles in the pattern
    for (const auto& item : pattern) {
        // Create the obstacle, setting exploding status based on texture
        bool isExploding = (item->textureIndex == 4);
        auto* obstacle = new BlockObstacle(this, isExploding);

        // Set scale
        obstacle->SetScale(Vector3(static_cast<float>(OBSTACLE_SCALE),
                                   static_cast<float>(OBSTACLE_SCALE),
                                   static_cast<float>(OBSTACLE_SCALE)));

        // Calculate y and z positions to center the 500x500 grid in the tunnel
        float yPos = (static_cast<float>(OBSTACLE_PATTERN_SIZE) / 2.0f - 0.5f - static_cast<float>(item->j)) *
                     static_cast<float>(OBSTACLE_SCALE);
        float zPos = (static_cast<float>(OBSTACLE_PATTERN_SIZE) / 2.0f - 0.5f - static_cast<float>(item->i)) *
                     static_cast<float>(OBSTACLE_SCALE);

        obstacle->SetPosition(Vector3(xPos, yPos, zPos));

        // Set the correct texture
        obstacle->SetTexture(item->textureIndex);
    }

    // Increment for the next wave
    mNextObstacle++;
}

void Game::SpawnWalls()
{
    // Create four blocks: left wall, right wall, floor and ceiling
    Block* leftWall = new Block(this);
    Block* rightWall = new Block(this);
    Block* floorBlock = new Block(this);
    Block* ceilingBlock = new Block(this);

    // Set uniform scale to 500.0f
    Vector3 wallScale(500.0f, 500.0f, 500.0f);
    leftWall->SetScale(wallScale);
    rightWall->SetScale(wallScale);
    floorBlock->SetScale(wallScale);
    ceilingBlock->SetScale(wallScale);

    // Set positions and textures as specified
    float baseX = static_cast<float>(mNextBlock) * 500.0f;
    leftWall->SetPosition(Vector3(baseX, 500.0f, 0.0f));
    leftWall->SetTexture(0);
    leftWall->SetRotation(Vector3(0.0f, Math::Pi, 0.0f));

    rightWall->SetPosition(Vector3(baseX, -500.0f, 0.0f));
    rightWall->SetTexture(0);

    floorBlock->SetPosition(Vector3(baseX, 0.0f, -500.0f));
    floorBlock->SetTexture(5);

    ceilingBlock->SetPosition(Vector3(baseX, 0.0f, 500.0f));
    ceilingBlock->SetTexture(6);

    // Increment next block index for subsequent calls
    mNextBlock++;
}

void Game::UpdateGame(float deltaTime)
{
    // Update all actors and pending actors
    UpdateActors(deltaTime);

    // Update UI screens
    for (auto ui : mUIStack) {
        if (ui->GetState() == UIScreen::UIState::Active) {
            ui->Update(deltaTime);
        }
    }

    // Delete any UI that are closed
    auto iter = mUIStack.begin();
    while (iter != mUIStack.end()) {
        if ((*iter)->GetState() == UIScreen::UIState::Closing) {
            delete *iter;
            iter = mUIStack.erase(iter);
        } else {
            ++iter;
        }
    }

    // Update game only if not paused
    if (!mIsPaused) {
        // Spawn new wall segments as the player advances every WALL_SCALE units
        if (mShip) {

            // Update HUD health
            if (mHUD) {
                mHUD->SetHealth(mShip->GetHealth());
            }

            // Compute the next threshold: when player's x exceeds this, spawn another group
            float blockSize = static_cast<float>(Game::WALL_SCALE);
            float threshold = (static_cast<float>(mNextBlock) - static_cast<float>(Game::TUNNEL_DEPTH) + 1.0f) * blockSize;
            // If threshold is negative (e.g., mNextBlock < TUNNEL_DEPTH), set to blockSize
            if (threshold < blockSize) threshold = blockSize;

            // Loop in case player advances more than one block in a single frame
            while (mShip->GetPosition().x > threshold) {
                SpawnWalls();
                threshold = (static_cast<float>(mNextBlock) - static_cast<float>(Game::TUNNEL_DEPTH) + 1.0f) * blockSize;
            }

            float obstacleThreshold = static_cast<float>(TUNNEL_DEPTH * WALL_SCALE) +
                                    static_cast<float>(mNextObstacle - OBSTACLES_WAVES_IN_SCREEN) * OBSTACLE_PATTERN_GAP;

            while (mShip->GetPosition().x > obstacleThreshold) {
                SpawnObstacles();

                int score = mNextObstacle - OBSTACLES_WAVES_IN_SCREEN;
                if (mHUD) {
                    mHUD->SetScore(score);
                }

                // Recalculate threshold for the next iteration
                obstacleThreshold = static_cast<float>(TUNNEL_DEPTH * WALL_SCALE) +
                                    static_cast<float>(mNextObstacle - OBSTACLES_WAVES_IN_SCREEN) * OBSTACLE_PATTERN_GAP;
            }
        }

        // Update camera to follow the ship (if camera exists)
        if (mCamera) {
            mCamera->Update(deltaTime, mShip);
        }
    }

    //  Update audio system
    if (mAudio) {
        mAudio->Update(deltaTime);
    }
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

void Game::AddObstacle(BlockObstacle* obstacle)
{
    mObstacles.emplace_back(obstacle);
}

void Game::RemoveObstacle(BlockObstacle* obstacle)
{
    std::vector<class BlockObstacle*>::iterator it;
    it = std::find(mObstacles.begin(), mObstacles.end(), obstacle);

    if (it != mObstacles.end())
    {
        mObstacles.erase(it);
    }
}

void Game::GenerateOutput()
{
    // Clear back buffer
    mRenderer->Clear();

    // Draw all drawables
    mRenderer->Draw();

    // Swap front buffer and back buffer
    mRenderer->Present();
}

void Game::Shutdown()
{
    // Because ~Actor calls RemoveActor, have to use a different style loop
    while (!mActors.empty()) {
        delete mActors.back();
    }

    // Delete UI screens
    for (auto ui : mUIStack) {
        delete ui;
    }
    mUIStack.clear();

    // Delete obstacle patterns
    for (auto pattern : mObstaclePatterns) {
        for (auto obstacle : pattern) {
            delete obstacle;
        }
    }
    mObstaclePatterns.clear();

    // Delete renderer
    mRenderer->Shutdown();
    delete mRenderer;
    mRenderer = nullptr;

    delete mAudio;
    mAudio = nullptr;

    SDL_DestroyWindow(mWindow);
    SDL_Quit();
}