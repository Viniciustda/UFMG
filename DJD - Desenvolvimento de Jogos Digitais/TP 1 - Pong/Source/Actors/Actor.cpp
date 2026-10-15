// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Actor.h"
#include "../Game.h"
#include <algorithm>

Actor::Actor(Game* game, int width, int height)
        : mState(ActorState::Active)
        , mPosition(Vector2::Zero)
        , mScale(1.0f)
        , mRotation(0.0f)
        , mGame(game)
        , mWidth(width)
        , mHeight(height)
        , mColor(Vector3(1.0f, 1.0f, 1.0f))
        , mVertexArray(nullptr)
{

}

Actor::~Actor()
{
    if (mVertexArray) {
        delete mVertexArray;
    }
}

void Actor::ProcessInput(const Uint8* keyState)
{
    if (mState == ActorState::Active) {
        OnProcessInput(keyState);
    }
}

void Actor::OnProcessInput(const Uint8* keyState)
{

}

void Actor::Update(float deltaTime)
{
    if (mState == ActorState::Active) {
        OnUpdate(deltaTime);
    }
}

void Actor::OnUpdate(float deltaTime)
{

}

Matrix4 Actor::GetModelMatrix() const
{
    // Scale
    float scaleX = mScale.x * static_cast<float>(mWidth);
    float scaleY = mScale.y * static_cast<float>(mHeight);
    Matrix4 scaleMat = Matrix4::CreateScale(scaleX, scaleY, 1.0f);

    // Rotation
    Matrix4 rotMat = Matrix4::CreateRotationZ(mRotation);

    // Translation
    Matrix4 transMat = Matrix4::CreateTranslation(
        Vector3(mPosition.x, mPosition.y, 0.0f)
    );

    return scaleMat * rotMat * transMat;
}

void Actor::Draw(Renderer* renderer)
{
    renderer->Draw(GetModelMatrix(), mColor, GetVertexArray());
}
