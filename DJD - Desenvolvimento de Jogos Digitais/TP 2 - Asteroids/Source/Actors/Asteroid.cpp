//
// Created by Lucas N. Ferreira on 10/09/23.
//

#define DB_PERLIN_IMPL

#include "Asteroid.h"
#include "../Game.h"
#include "../Random.h"
#include "../Components/CircleColliderComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/DrawComponent.h"

Asteroid::Asteroid(Game* game, const float radius, const bool isLarge, const int numVertices, const float forwardForce)
        :Actor(game)
        ,mRigidBodyComponent(nullptr)
        ,mDrawComponent(nullptr)
        ,mIsLarge(isLarge)
        ,mRadius(radius)
{
        // Generate vertices for the asteroid
        std::vector<Vector2> vertices = GenerateVertices(numVertices, radius);

        // Calculate the average radius for the collider
        float avgRadius = CalculateAverageVerticesLength(vertices);

        // Create components
        mDrawComponent = new DrawComponent(this, vertices);
        mRigidBodyComponent = new RigidBodyComponent(this, 1.0f);
        mCircleColliderComponent = new CircleColliderComponent(this, avgRadius);

        // Apply a random starting force
        Vector2 randomForce = GenerateRandomStartingForce(forwardForce, forwardForce * 4.0f);
        mRigidBodyComponent->ApplyForce(randomForce);

        // Add this asteroid to the game's list
        GetGame()->AddAsteroid(this);
}

Asteroid::~Asteroid()
{
        GetGame()->RemoveAsteroid(this);

        GetGame()->Explosion(GetPosition());
        
        if (mIsLarge) {
                for (int i = 0; i < 3; ++i) {
                        // Spawn a smaller, faster asteroid
                        auto* ast = new Asteroid(GetGame(), Random::GetFloatRange(mRadius * 0.4f, mRadius * 0.7f), false, Random::GetIntRange(6, 10), 3000.0f);
                        ast->SetPosition(GetPosition());
                }
        }
}

std::vector<Vector2> Asteroid::GenerateVertices(const int numVertices, const float radius)
{
        std::vector<Vector2> vertices;
        vertices.reserve(numVertices);

        float angleStep = Math::TwoPi / static_cast<float>(numVertices);

        for (int i = 0; i < numVertices; ++i) {
                float angle = static_cast<float>(i) * angleStep;
                float randomRadius = Random::GetFloatRange(radius * 0.6f, radius * 1.4f);

                float x = randomRadius * Math::Cos(angle);
                float y = randomRadius * Math::Sin(angle);

                vertices.emplace_back(Vector2(x, y));
        }

        return vertices;
}

float Asteroid::CalculateAverageVerticesLength(std::vector<Vector2>& vertices)
{
        float sum = 0.0f;
        for (const auto& v : vertices) {
                sum += v.Length();
        }
        return sum / static_cast<float>(vertices.size());
}

Vector2 Asteroid::GenerateRandomStartingForce(const float min, const float max)
{
        float angle = Random::GetFloatRange(0.0f, Math::TwoPi);
        float magnitude = Random::GetFloatRange(min, max);
        return Vector2(Math::Cos(angle), Math::Sin(angle)) * magnitude;
}