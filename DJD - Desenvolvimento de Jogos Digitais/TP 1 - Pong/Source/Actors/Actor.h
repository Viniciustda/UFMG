// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include <vector>
#include <SDL_stdinc.h>
#include "../Math.h"
#include "../Renderer/Renderer.h"
#include "../Renderer/Shader.h"
#include "../Renderer/VertexArray.h"

enum class ActorState
{
    Active,
    Paused,
    Destroy
};

class Actor
{
public:
    Actor(class Game* game, int width = 0, int height = 0);
    virtual ~Actor();

    // Update function called from Game (not overridable)
    void Update(float deltaTime);

    // ProcessInput function called from Game (not overridable)
    void ProcessInput(const Uint8* keyState);

    virtual void Draw(Renderer* renderer);

    // Position getter/setter
    const Vector2& GetPosition() const { return mPosition; }
    void SetPosition(const Vector2& pos) { mPosition = pos; }

    // Scale getter/setter
    const Vector2& GetScale() const { return mScale; }
    void SetScale(const Vector2& scale) { mScale = scale; }

    // Rotation getter/setter
    float GetRotation() const { return mRotation; }
    void SetRotation(float rotation) { mRotation = rotation; }

    // State getter/setter
    ActorState GetState() const { return mState; }
    void SetState(ActorState state) { mState = state; }

    // Color getter/setter
    const Vector3& GetColor() const { return mColor; }
    void SetColor(const Vector3& color) { mColor = color; }

    // Vertex array creation 
    virtual void CreateVertexArray() = 0;
    VertexArray* GetVertexArray() { return mVertexArray; }

    // Dimensions getter/setter
    int GetWidth() const { return mWidth; }
    int GetHeight() const { return mHeight; }

    // Returns the world transform matrix
    Matrix4 GetModelMatrix() const;

    // Game getter
    class Game* GetGame() { return mGame; }

protected:
    class Game* mGame;

    // Any actor-specific update code (overridable)
    virtual void OnUpdate(float deltaTime);
    // Any actor-specific update code (overridable)
    virtual void OnProcessInput(const Uint8* keyState);

    // Actor's state
    ActorState mState;

    // Transform
    Vector2 mPosition;
    Vector2 mScale;
    float mRotation;

    Vector3 mColor;
    VertexArray* mVertexArray;

    int mWidth;
    int mHeight;
};