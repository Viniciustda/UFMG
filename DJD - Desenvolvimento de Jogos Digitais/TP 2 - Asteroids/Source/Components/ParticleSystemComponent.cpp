//
// Created by Lucas N. Ferreira on 18/09/25.
//

#include "ParticleSystemComponent.h"
#include "../Game.h"
#include "../Actors/Asteroid.h"
#include "../Components/CircleColliderComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/DrawComponent.h"
#include "../Random.h"

Particle::Particle(class Game* game, std::vector<Vector2> &vertices, Vector3 color, bool collidable)
    : Actor(game)
    , mDrawComponent(nullptr)
    , mRigidBodyComponent(nullptr)
    , mCircleColliderComponent(nullptr)
    , mIsDead(true)
    , mLifeTime(1.0f)
    , mCollidable(collidable)
{
    mDrawComponent = new DrawComponent(this, vertices);
    mDrawComponent->SetColor(color);
    mRigidBodyComponent = new RigidBodyComponent(this);
    if (mCollidable) {
        mCircleColliderComponent = new CircleColliderComponent(this, 2.0f);
    }

    SetState(ActorState::Paused);
    mDrawComponent->SetVisible(false);
    mIsDead = true;
}

void Particle::Kill()
{
    mIsDead = true;
    SetState(ActorState::Paused);
    mDrawComponent->SetVisible(false);
    mRigidBodyComponent->SetVelocity(Vector2::Zero);
}

void Particle::Awake(const Vector2 &position, float rotation, float lifetime)
{
    mLifeTime = lifetime;
    mIsDead = false;
    SetState(ActorState::Active);
    mDrawComponent->SetVisible(true);
    SetPosition(position);
    SetRotation(rotation);
}

void Particle::OnUpdate(float deltaTime)
{
    mLifeTime -= deltaTime;
    if (mLifeTime <= 0.0f) {
        Kill();
        return;
    }

    if (mCollidable) {
        for (auto asteroid : GetGame()->GetAsteroids()) {
            if (mCircleColliderComponent->Intersect(*asteroid->GetComponent<CircleColliderComponent>())) {
                Kill();
                asteroid->SetState(ActorState::Destroy);
                break;
            }
        }
    }
}

ParticleSystemComponent::ParticleSystemComponent(class Actor* owner, std::vector<Vector2> &vertices, int poolSize, int updateOrder, Vector3 color, bool collidable)
    : Component(owner, updateOrder)
{
    mParticles.reserve(poolSize);
    for (int i = 0; i < poolSize; ++i) {
        mParticles.emplace_back(new Particle(owner->GetGame(), vertices, color, collidable));
    }
}

void ParticleSystemComponent::EmitParticle(float lifetime, float speed, ParticleDirection direction, const Vector2& offsetPosition)
{
    for (auto p : mParticles) {
        if (p->IsDead()) {
            Vector2 spawnPos = mOwner->GetPosition() + offsetPosition;
            Vector2 forceDirection;
            float rotation = 0.0f;

            switch (direction) {
                case ParticleDirection::Random:
                {
                    float angle = Random::GetFloatRange(0.0f, Math::TwoPi);
                    forceDirection = Vector2(Math::Cos(angle), Math::Sin(angle));
                    rotation = angle;
                    break;
                }
                case ParticleDirection::Forward:
                {
                    forceDirection = mOwner->GetForward();
                    rotation = mOwner->GetRotation();
                    break;
                }
            }

            p->Awake(spawnPos, rotation, lifetime);

            auto rb = p->GetComponent<RigidBodyComponent>();
            if (rb) {
                rb->ApplyForce(forceDirection * speed);
            }
            break;
        }
    }
}