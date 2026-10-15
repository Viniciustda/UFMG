//
// Created by Lucas N. Ferreira on 18/09/25.
//

#pragma once

#include "../Actors/Actor.h"
#include "Component.h"
#include <vector>

enum class ParticleDirection
{
    Random,
    Forward
};

class Particle : public Actor
{
public:
    Particle(class Game* game, std::vector<Vector2> &vertices, Vector3 color, bool collidable);

    void OnUpdate(float deltaTime) override;

    bool IsDead() const { return mIsDead; }
    void Awake(const Vector2 &position, float rotation, float lifetime = 1.0f);
    void Kill();

private:
    float mLifeTime;
    bool mIsDead;
    bool mCollidable;

    class DrawComponent* mDrawComponent;
    class RigidBodyComponent* mRigidBodyComponent;
    class CircleColliderComponent* mCircleColliderComponent;
};

class ParticleSystemComponent : public Component {

public:
    ParticleSystemComponent(class Actor* owner, std::vector<Vector2> &vertices,  int poolSize = 100, int updateOrder = 10, Vector3 color = Vector3(1.0f, 1.0f, 1.0f), bool collidable = true);
    void EmitParticle(float lifetime, float speed, ParticleDirection direction, const Vector2& offsetPosition = Vector2::Zero);

private:
    std::vector<class Particle*> mParticles;
};
