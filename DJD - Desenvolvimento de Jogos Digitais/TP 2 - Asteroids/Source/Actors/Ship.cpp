//
// Created by Lucas N. Ferreira on 03/08/23.
//

#include "Ship.h"
#include "Asteroid.h"
#include "../Game.h"
#include "../Components/CircleColliderComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/DrawComponent.h"
#include "../Components/ParticleSystemComponent.h"
#include "../Random.h"

Ship::Ship(Game* game,
           const float height,
           const float forwardForce,
           const float rotationForce)
        : Actor(game)
        , mLaserCooldown(0.f)
        , mHeight(height)
        , mRigidBodyComponent(nullptr)
        , mDrawComponent(nullptr)
        , mThrusterDrawComponent(nullptr)
        , mForwardSpeed(forwardForce)
        , mRotationForce(rotationForce)
        , mWeapon(nullptr)
{
        // Create a triangle for the ship
        std::vector<Vector2> vertices;
        vertices.emplace_back(Vector2(height / 2.0f, 0.0f));          // Tip
        vertices.emplace_back(Vector2(-height / 2.0f, height / 3.0f));  // Bottom right
        vertices.emplace_back(Vector2(-height / 3.0f, height / 6.0f));  // Inner right
        vertices.emplace_back(Vector2(-height / 3.0f, -height / 6.0f)); // Inner left
        vertices.emplace_back(Vector2(-height / 2.0f, -height / 3.0f));  // Bottom left

        // Create components
        mDrawComponent = new DrawComponent(this, vertices);
        mDrawComponent->SetColor(Vector3(0.0f, 1.0f, 1.0f)); // Cyan
        mRigidBodyComponent = new RigidBodyComponent(this, 1.5f);
        mCircleColliderComponent = new CircleColliderComponent(this, height / 4.0f);

        // Create thruster
        std::vector<Vector2> thrusterVertices;
        thrusterVertices.emplace_back(Vector2(-height / 3.0f, height / 12.0f));
        thrusterVertices.emplace_back(Vector2(-height / 1.5f, 0.0f));
        thrusterVertices.emplace_back(Vector2(-height / 3.0f, -height / 12.0f));
        mThrusterDrawComponent = new DrawComponent(this, thrusterVertices);
        mThrusterDrawComponent->SetColor(Vector3(1.0f, 0.0f, 0.0f)); // Red
        mThrusterDrawComponent->SetVisible(false);

        // Create weapon
        std::vector<Vector2> laserVertices;
        laserVertices.emplace_back(Vector2(0.f, 1.f));
        laserVertices.emplace_back(Vector2(5.f, 1.f));
        laserVertices.emplace_back(Vector2(5.f, -1.f));
        laserVertices.emplace_back(Vector2(0.f, -1.f));
        mWeapon = new ParticleSystemComponent(this, laserVertices, 10, 10, Vector3(1.0f, 1.0f, 0.0f)); // Yellow
}

void Ship::OnProcessInput(const uint8_t* state)
{
        if (state[SDL_SCANCODE_W]) {
                mRigidBodyComponent->ApplyForce(GetForward() * mForwardSpeed);
                mThrusterDrawComponent->SetVisible(Random::GetFloatRange(-1.0f, 1.0f) > 0.1f);
        }
        else {
                mThrusterDrawComponent->SetVisible(false);
        }

        float angularSpeed = 0.0f;
        if (state[SDL_SCANCODE_A]) {
                angularSpeed -= mRotationForce;
        }
        if (state[SDL_SCANCODE_D]) {
                angularSpeed += mRotationForce;
        }
        mRigidBodyComponent->SetAngularSpeed(angularSpeed);

        if (state[SDL_SCANCODE_SPACE] && mLaserCooldown <= 0.0f) {
            mWeapon->EmitParticle(0.4f, 100000.0f, ParticleDirection::Forward, GetForward() * (mHeight / 2.0f));
            mLaserCooldown = 0.3f;
        }
}

void Ship::OnUpdate(float deltaTime)
{
        if (mLaserCooldown > 0.0f) {
                mLaserCooldown -= deltaTime;
        }
        if (mLaserCooldown < 0.0f) {
                mLaserCooldown = 0.0f;
        }

        for (auto asteroid : GetGame()->GetAsteroids()) {
                if (mCircleColliderComponent->Intersect(*asteroid->GetComponent<CircleColliderComponent>())) {
                        GetGame()->Quit();
                        break;
                }
        }
}