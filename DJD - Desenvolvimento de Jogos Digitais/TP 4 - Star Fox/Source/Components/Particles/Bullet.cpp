//
// Created by Lucas N. Ferreira on 04/11/25.
//

#include "Bullet.h"
#include "../../Actors/Actor.h"
#include "../../Game.h"
#include "../Drawing/MeshComponent.h"
#include "../Physics/AABBColliderComponent.h"
#include "../Physics/RigidBodyComponent.h"
#include "../../Actors/BlockObstacle.h"


Bullet::Bullet(class Game* game)
        :Particle(game)
{
    // Add mesh component
    mMeshComponent = new MeshComponent(this);
    auto* mesh = GetGame()->GetRenderer()->GetMesh("../Assets/Laser.gpmesh");
    mMeshComponent->SetMesh(mesh);
    mMeshComponent->SetVisible(false);

    // Set scale
    SetScale(Vector3(5.0f, 5.0f, 5.0f));

    // Add AABB collider component
    mCollisionComponent = new AABBColliderComponent(this, 10.0f, 10.0f, 10.0f, Vector3::Zero, ColliderLayer::Bullet);
    mCollisionComponent->SetEnabled(false);

    // Add rigid body component
    mRigidBody = new RigidBodyComponent(this, 1.0f, 0.0f);
    mRigidBody->SetVelocity(Vector3::Zero);
}

Bullet::~Bullet()
{
}

void Bullet::Kill()
{
    Particle::Kill();

    mMeshComponent->SetVisible(false);
    mCollisionComponent->SetEnabled(false);
    mRigidBody->SetVelocity(Vector3::Zero);
}

void Bullet::Awake(const Vector3 &position, const Vector3 &rotation, float lifetime)
{
    Particle::Awake(position, rotation, lifetime);

    mCollisionComponent->SetEnabled(true);
    mMeshComponent->SetVisible(true);
}

void Bullet::Emit(const Vector3 &direction, float speed)
{
    mRigidBody->SetVelocity(direction * speed);
}

void Bullet::OnUpdate(float deltaTime)
{
    Particle::OnUpdate(deltaTime);

    // Check for collision with obstacles
    for (auto obstacle : mGame->GetObstacles()) {
        if (mCollisionComponent->Intersect(obstacle->GetCollider())) {
            // Kill the bullet
            Kill();

            // If the obstacle is explosive, explode it
            if (obstacle->IsExploding()) {
                obstacle->Explode();
            }

            // A bullet can only hit one thing
            break;
        }
    }
}