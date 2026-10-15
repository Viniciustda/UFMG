//
// Created by Lucas N. Ferreira on 08/09/23.
//

#include <SDL.h>
#include "RigidBodyComponent.h"
#include "../Actors/Actor.h"
#include "../Game.h"

RigidBodyComponent::RigidBodyComponent(class Actor* owner, float mass, int updateOrder)
        :Component(owner, updateOrder)
        ,mMass(mass)
        ,mAngularSpeed(0.0f)
        ,mVelocity(Vector2::Zero)
        ,mAcceleration(Vector2::Zero)
{

}

void RigidBodyComponent::ApplyForce(const Vector2 &force)
{
        mAcceleration += force * (1.0f / mMass);
}

void RigidBodyComponent::Update(float deltaTime)
{
        // Update velocity based on acceleration
        mVelocity += mAcceleration * deltaTime;

        // Limit max velocity
        if (mVelocity.Length() > MAX_VELOCITY) {
        mVelocity.Normalize();
        mVelocity *= MAX_VELOCITY;
        }

        // Update position based on velocity
        Vector2 pos = mOwner->GetPosition();
        pos += mVelocity * deltaTime;
        ScreenWrap(pos);
        mOwner->SetPosition(pos);

        // Reset acceleration
        mAcceleration = Vector2::Zero;

        // Correct very small velocities
        if (Math::NearlyZero(mVelocity.Length())) {
        mVelocity = Vector2::Zero;
        }

        // Update rotation
        float rot = mOwner->GetRotation();
        rot += mAngularSpeed * deltaTime;
        mOwner->SetRotation(rot);
}

void RigidBodyComponent::ScreenWrap(Vector2 &position)
{
        if (position.x < 0.0f) {
                position.x = Game::WINDOW_WIDTH;
        }
        else if (position.x > Game::WINDOW_WIDTH) {
                position.x = 0.0f;
        }

        if (position.y < 0.0f) {
                position.y = Game::WINDOW_HEIGHT;
        }
        else if (position.y > Game::WINDOW_HEIGHT) {
                position.y = 0.0f;
        }
}
