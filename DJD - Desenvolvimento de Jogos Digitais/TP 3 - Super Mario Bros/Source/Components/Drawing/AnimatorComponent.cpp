//
// Created by Lucas N. Ferreira on 28/09/23.
//

#include "AnimatorComponent.h"
#include "../../Actors/Actor.h"
#include "../../Game.h"
#include "../../Json.h"
#include "../../Renderer/Texture.h"
#include <fstream>

AnimatorComponent::AnimatorComponent(class Actor* owner, const std::string &texPath, const std::string &dataPath,
                                     int width, int height, int drawOrder)
        :DrawComponent(owner,  drawOrder)
        ,mAnimTimer(0.0f)
        ,mIsPaused(false)
        ,mWidth(width)
        ,mHeight(height)
        ,mTextureFactor(1.0f)
{
    mSpriteTexture = mOwner->GetGame()->GetRenderer()->GetTexture(texPath);

    if (!dataPath.empty()) {
        LoadSpriteSheetData(dataPath);
    }
}

AnimatorComponent::~AnimatorComponent()
{
    mAnimations.clear();
    mSpriteSheetData.clear();
}

bool AnimatorComponent::LoadSpriteSheetData(const std::string& dataPath)
{
    // Load sprite sheet data and return false if it fails
    std::ifstream spriteSheetFile(dataPath);

    if (!spriteSheetFile.is_open()) {
        SDL_Log("Failed to open sprite sheet data file: %s", dataPath.c_str());
        return false;
    }

    nlohmann::json spriteSheetData = nlohmann::json::parse(spriteSheetFile);

    if (spriteSheetData.is_null()) {
        SDL_Log("Failed to parse sprite sheet data file: %s", dataPath.c_str());
        return false;
    }

    auto textureWidth = static_cast<float>(spriteSheetData["meta"]["size"]["w"].get<int>());
    auto textureHeight = static_cast<float>(spriteSheetData["meta"]["size"]["h"].get<int>());

    for(const auto& frame : spriteSheetData["frames"]) {

        int x = frame["frame"]["x"].get<int>();
        int y = frame["frame"]["y"].get<int>();
        int w = frame["frame"]["w"].get<int>();
        int h = frame["frame"]["h"].get<int>();

        mSpriteSheetData.emplace_back(static_cast<float>(x)/textureWidth, static_cast<float>(y)/textureHeight,
                                      static_cast<float>(w)/textureWidth, static_cast<float>(h)/textureHeight);
    }

    return true;
}

void AnimatorComponent::Draw(Renderer* renderer)
{
    if (mIsVisible && mSpriteTexture) {
        Vector4 texRect;

        // If there is an active animation
        if (!mAnimName.empty() && !mAnimations.empty())
        {
            // Get the current frame index from the timer
            int frameIndex = static_cast<int>(mAnimTimer);
            // Get the list of sprite indices for the current animation
            const auto& animFrames = mAnimations[mAnimName];
            // Get the actual sprite index from the animation list
            int spriteIndex = animFrames[frameIndex];
            // Get the texture coordinates for the current sprite
            texRect = mSpriteSheetData[spriteIndex];
        }
        // If no animation, but there is a sprite sheet, use the first frame
        else if (!mSpriteSheetData.empty())
        {
            texRect = mSpriteSheetData[0];
        }
        // If no sprite sheet data at all, use the full texture
        else
        {
            texRect = Vector4(0.0f, 0.0f, 1.0f, 1.0f);
        }

        // Check if we need to flip the texture based on the owner's scale
        bool shouldFlip = mOwner->GetScale().x < 0.0f;

        // Calculate the draw position with the offset
        Vector2 drawPos = mOwner->GetPosition() + mDrawOffset;

        // Calculate the final size, taking the owner's scale into account
        Vector2 finalSize = Vector2(
            static_cast<float>(mWidth),
            static_cast<float>(mHeight) * mOwner->GetScale().y
        );

        renderer->DrawTexture(
                drawPos,
                finalSize,
                mOwner->GetRotation(),
                Vector3(1.0f, 1.0f, 1.0f),
                mSpriteTexture,
                texRect,
                mOwner->GetGame()->GetCameraPos(),
                shouldFlip,
                mTextureFactor
        );
    }
}

void AnimatorComponent::Update(float deltaTime)
{
    if (mIsPaused || mAnimations.empty() || mAnimName.empty()) {
        return;
    }

    // Update animation timer
    mAnimTimer += mAnimFPS * deltaTime;

    // Get the number of frames in the current animation
    float numFrames = static_cast<float>(mAnimations[mAnimName].size());

    // Wrap the timer if it exceeds the number of frames
    if (mAnimTimer >= numFrames) {
        mAnimTimer -= numFrames;
    }
}

void AnimatorComponent::SetAnimation(const std::string& name)
{
    // Only change the animation if it's a new one
    if (mAnimName != name) {
        mAnimName = name;
        // Reset the timer to the start of the new animation
        mAnimTimer = 0.0f;
    }
}

void AnimatorComponent::AddAnimation(const std::string& name, const std::vector<int>& spriteNums)
{
    mAnimations.emplace(name, spriteNums);
}

void AnimatorComponent::SetSize(float width, float height)
{
    mWidth = width;
    mHeight = height;
}

void AnimatorComponent::SetOffset(const Vector2& offset)
{
    mDrawOffset = offset;
}

void AnimatorComponent::SetSpriteSheet(const std::string& texturePath, const std::string& dataPath)
{
    // Load new texture
    if (!mOwner || !mOwner->GetGame()) return;

    Texture* tex = mOwner->GetGame()->GetRenderer()->GetTexture(texturePath);
    mSpriteTexture = tex;

    mAnimations.clear();
    mSpriteSheetData.clear();
    mAnimName.clear();

    if (!dataPath.empty()) {
        LoadSpriteSheetData(dataPath);
    }
}