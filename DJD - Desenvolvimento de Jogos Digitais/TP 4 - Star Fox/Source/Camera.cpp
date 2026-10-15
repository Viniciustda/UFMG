//
// Created by Lucas N. Ferreira on 03/11/25.
//

#include "Camera.h"
#include "Game.h"
#include "Actors/Actor.h"

Camera::Camera(class Game* game, const Vector3 &eye, const Vector3 &target, const Vector3 &up,
        float foy, float near, float far)
      :mGame(game)
      ,mEye(eye)
      ,mTarget(target)
      ,mUp(up)
      ,mFovY(foy)
      ,mNear(near)
      ,mFar(far)
      ,mHDistance(300.0f)
      ,mVDistance(0.0f)
      ,mTDistance(20.0f)
{
  // Create initial view matrix and projection (perspective) matrix
  mViewMatrix = Matrix4::CreateLookAt(mEye, mTarget, mUp);

  // fov passed in degrees -> convert to radians for CreatePerspectiveFOV
  float fovRad = Math::ToRadians(mFovY);
  // Use game's window size for aspect (CreatePerspectiveFOV expects width/height)
  mPerspProjMatrix = Matrix4::CreatePerspectiveFOV(fovRad, static_cast<float>(Game::WINDOW_WIDTH),
                           static_cast<float>(Game::WINDOW_HEIGHT), mNear, mFar);

  // Pass matrices to renderer
  if (mGame && mGame->GetRenderer()) {
    mGame->GetRenderer()->SetViewMatrix(mViewMatrix);
    mGame->GetRenderer()->SetProjectionMatrix(mPerspProjMatrix);
  }
}

void Camera::Update(float deltaTime, Actor *targetActor)
{
  // If no target actor, nothing to do
  if (targetActor == nullptr) {
    return;
  }

  // Get player's position and forward direction
  Vector3 playerPos = targetActor->GetPosition();
  Vector3 forward = targetActor->GetForward();

  // Compute the point to look at: a point mTDistance in front of the player
  mTarget = playerPos + forward * mTDistance;

  // Compute camera eye position: behind the player by mHDistance and offset along mUp by mVDistance
  mEye = playerPos - forward * mHDistance + mUp * mVDistance;

  // Force camera vertical position (z) to zero so the camera stays at z=0
  mEye.z = 0.0f;

  // Recompute view matrix and update renderer
  mViewMatrix = Matrix4::CreateLookAt(mEye, mTarget, mUp);

  if (mGame && mGame->GetRenderer()) {
    mGame->GetRenderer()->SetViewMatrix(mViewMatrix);
  }
}