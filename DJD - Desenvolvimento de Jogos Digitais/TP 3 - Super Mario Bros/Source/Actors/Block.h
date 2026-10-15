//
// Created by Lucas N. Ferreira on 28/09/23.
//

#pragma once

#include "Actor.h"

class Block : public Actor
{
public:
    Block(class Game* game, const std::string &texturePath, bool hasMushroom = false);

    void OnUpdate(float deltaTime) override;
    void Bump();
    const std::string& GetTexturePath() const { return mTexturePath; }

private:
    std::string mTexturePath;
    Vector2 mOriginalPos;
    bool mIsBumping;
    float mBumpSpeed;
    const float mBumpHeight = 8.0f;
    bool mIsUsed;
    bool mHasMushroom;
    int mFrameIndex;
};