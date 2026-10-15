//
// Created by Lucas N. Ferreira on 08/09/23.
//

#pragma once
#include "Actor.h"

class Mario : public Actor
{
public:
    explicit Mario(Game* game, float forwardSpeed = 1500.0f, float jumpSpeed = -750.0f);

    void OnProcessInput(const Uint8* keyState) override;
    void OnUpdate(float deltaTime) override;

    void OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other) override;
    void OnVerticalCollision(const float minOverlap, AABBColliderComponent* other) override;

    void Kill() override;
    void PowerUp();
    void PowerDown();

private:
    void ManageAnimations();

    float mForwardSpeed;
    float mJumpSpeed;
    bool mIsRunning;
    bool mIsDead;
    bool mIsSuper;
    float mInvincibilityTimer;
    float mStompTimer;   // mostra animação de stomp por um curto período
   float mTurnTimer;    // mostra animação de turn quando troca de direção parado
   float mPrevScaleX;

    class RigidBodyComponent* mRigidBodyComponent;
    class AnimatorComponent* mDrawComponent;
    class AABBColliderComponent* mColliderComponent;
    class ParticleSystemComponent* mFireBalls;

    float mFireBallCooldown = 1.0f;
};