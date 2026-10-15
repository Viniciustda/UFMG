//
// Created by Lucas N. Ferreira on 02/11/25.
//

#include "Ship.h"
#include "../Actors/BlockObstacle.h"
#include "../Game.h"
#include "../Components/Drawing/MeshComponent.h"
#include "../Components/Physics/AABBColliderComponent.h"
#include "../Components/Physics/RigidBodyComponent.h"
#include "../UI/Screens/HUD.h"
#include "../UI/Screens/GameOver.h"
#include <algorithm>

Ship::Ship(class Game* game)
        : Actor(game)
        , mMeshComponent(nullptr)
        , mCollisionComponent(nullptr)
        , mRigidBody(nullptr)
        , mLaserCooldown(0.f)
        , mInvinsibleCooldown(0.f)
        , mHealth(3)
        , mIsInvincible(false)
        , mTurnDirection(0.0f)
        ,mTurnAcceleration(0.0f)
        ,mTurnVelocity(0.0f)
        ,mSpeed(700.0f)
{
    // Add mesh component
    mMeshComponent = new MeshComponent(this);
    auto* mesh = GetGame()->GetRenderer()->GetMesh("../Assets/Arwing.gpmesh");
    mMeshComponent->SetMesh(mesh);

    // Add AABB collider component
    mCollisionComponent = new AABBColliderComponent(this, 15.0f, 40.0f, 25.0f, Vector3::Zero, ColliderLayer::Player);

    // Add rigid body component
    mRigidBody = new RigidBodyComponent(this, 1.0f, 2.0f);
    mRigidBody->SetVelocity(Vector3(mSpeed, 0.0f, 0.0f));

    // Add particle system component
    mLaserWeapon = new ParticleSystemComponent<Bullet>(this, 50);

    // Set scale
    SetScale(Vector3(2.0f, 2.0f, 2.0f));

    // Play ship loop sound
    mShipSound = mGame->GetAudio()->PlaySound("ShipLoop.ogg", true);
}

Ship::~Ship()
{
}

void Ship::DealDamage(const int damage)
{
    // Return if the ship is currently invincible
    if (mIsInvincible) {
        return;
    }

    // Deal damage and become invincible
    mHealth -= damage;
    mIsInvincible = true;
    mInvinsibleCooldown = 2.0f;

    // Check for game over
    if (mHealth <= 0) {
        SetState(ActorState::Paused);
        new GameOver(mGame, "../Assets/Fonts/Arial.ttf");

        // Play ship die sound
        mGame->GetAudio()->PlaySound("ShipDie.wav");

        // Stop ship and alert sounds
        mGame->GetAudio()->StopSound(mShipSound);
        if (mAlertSound.IsValid()) {
            mGame->GetAudio()->StopSound(mAlertSound);
            mAlertSound.Reset();
        }
    }
    else {
        // Play ship hit sound
        mGame->GetAudio()->PlaySound("ShipHit.wav");

        // Play alert sound if not already playing
        if (mHealth == 1 && !mAlertSound.IsValid()) {
            mAlertSound = mGame->GetAudio()->PlaySound("DamageAlert.ogg", true);
        }
        else if (mHealth > 1 && mAlertSound.IsValid()) {
            mGame->GetAudio()->StopSound(mAlertSound);
            mAlertSound.Reset();
        }
    }
}

void Ship::SetSpeed(const float speed)
{
    mSpeed = speed;
    mRigidBody->SetVelocity(Vector3(mSpeed, 0.0f, 0.0f));
}

void Ship::OnUpdate(float deltaTime)
{
    // Define rotation parmeters
    const float interpolationSpeed = 10.0f;
    const float maxPitchAngle = Math::ToRadians(15.0f); 
    const float maxRollAngle = Math::ToRadians(30.0f);  
    
    // Define the velocity at which the maximum rotation occurs
    const float velocityForMaxRotation = 200.0f;

    // Get the current velocity of the RigidBody
    const Vector3& velocity = mRigidBody->GetVelocity();
    
    // Pitch (tilt up/down) is rotation around the Y axis, based on Z velocity
    float targetPitch = -(velocity.z / velocityForMaxRotation) * maxPitchAngle;
    targetPitch = Math::Clamp(targetPitch, -maxPitchAngle, maxPitchAngle);

    // Roll (tilt left/right) is rotation around the X axis, based on Y velocity
    float targetRoll = -(velocity.y / velocityForMaxRotation) * maxRollAngle;
    targetRoll = Math::Clamp(targetRoll, -maxRollAngle, maxRollAngle);

    // Create the target rotation Euler angles vector
    Vector3 targetAngles = Vector3(targetRoll, targetPitch, 0.0f); 

    // Get the current rotation (in Euler angles)
    Vector3 currentAngles = GetRotation(); 

    // Interpolate smoothly from the current rotation to the target rotation using Lerp
    Vector3 newAngles = Vector3::Lerp(currentAngles, targetAngles, deltaTime * interpolationSpeed); 

    // Apply the new rotation to the ship
    SetRotation(newAngles);

    // Clamp horizontal (y) and vertical (z) positions
    Vector3 pos = GetPosition();
    float clampedY = std::clamp(pos.y, -180.0f, 180.0f);
    float clampedZ = std::clamp(pos.z, -225.0f, 225.0f);

    if (clampedY != pos.y || clampedZ != pos.z) {
        SetPosition(Vector3(pos.x, clampedY, clampedZ));
    }

    // Handle invincibility cooldown
    if (mIsInvincible) {
        mInvinsibleCooldown -= deltaTime;
        if (mInvinsibleCooldown <= 0.0f) {
            mIsInvincible = false;
        }
    }

    // Check for collisions with obstacles
    for (auto obstacle : mGame->GetObstacles()) {
        if (mCollisionComponent->Intersect(obstacle->GetCollider())) {
            // Explode the obstacle
            obstacle->Explode();

            // Deal damage to the ship (invincibility is handled inside)
            DealDamage(1);
        }
    }

    if (mLaserCooldown > 0.0f) {
        mLaserCooldown -= deltaTime;
    }
}

void Ship::OnProcessInput(const Uint8 *keyState)
{
    const float forceAmount = 1000.0f;

    if (!mRigidBody) return;

    // Up (W) -> +z
    if (keyState[SDL_SCANCODE_W]) {
        mRigidBody->ApplyForce(Vector3(0.0f, 0.0f, forceAmount));
    }

    // Down (S) -> -z
    if (keyState[SDL_SCANCODE_S]) {
        mRigidBody->ApplyForce(Vector3(0.0f, 0.0f, -forceAmount));
    }

    // Left (A) -> -y
    if (keyState[SDL_SCANCODE_A]) {
        mRigidBody->ApplyForce(Vector3(0.0f, -forceAmount, 0.0f));
    }

    // Right (D) -> +y
    if (keyState[SDL_SCANCODE_D]) {
        mRigidBody->ApplyForce(Vector3(0.0f, forceAmount, 0.0f));
    }

    // Spacebar -> Fire Laser
    if (keyState[SDL_SCANCODE_SPACE]) {
        // If the laser cooldown has finished
        if (mLaserCooldown <= 0.0f) {
            // Set the cooldown (frequency of 0.25s)
            mLaserCooldown = 0.25f;

            // Play shoot sound
            mGame->GetAudio()->PlaySound("Shoot.wav");

            float lifetime = 1.0f;                          // 1 second lifetime
            float speed = GetSpeed() * 2.0f;                // Double the ship's speed
            const float spawnOffset = 100.0f;                 // Offset from the ship's position
            Vector3 offset = GetForward() * spawnOffset;    // Direction is the ship's forward vector
            
            // Emit the laser particle
            mLaserWeapon->EmitParticle(lifetime, speed, offset);
        }
    }
}