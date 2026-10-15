//
// Created by Lucas N. Ferreira on 10/09/23.
//

#include "CircleColliderComponent.h"
#include "../Actors/Actor.h"

CircleColliderComponent::CircleColliderComponent(class Actor* owner, const float radius, const int updateOrder)
        :Component(owner, updateOrder)
        ,mRadius(radius)
{
        const int numVertices = 10;
        std::vector<float> vertexBuffer;
        vertexBuffer.reserve(numVertices);

        for (int i = 0; i < numVertices; ++i) {
                float angle = (static_cast<float>(i) / numVertices) * Math::TwoPi;
                vertexBuffer.emplace_back(mRadius * Math::Cos(angle));
                vertexBuffer.emplace_back(mRadius * Math::Sin(angle));
        }

        std::vector<unsigned int> indexBuffer;
        for (int i = 0; i < numVertices - 1; ++i) {
                indexBuffer.emplace_back(i);
                indexBuffer.emplace_back(i + 1);
        }
        indexBuffer.emplace_back(numVertices - 1);
        indexBuffer.emplace_back(0);

        mDrawArray = new VertexArray(vertexBuffer.data(), numVertices, indexBuffer.data(), indexBuffer.size());
}

CircleColliderComponent::~CircleColliderComponent()
{
        delete mDrawArray;
        mDrawArray = nullptr;
}

bool CircleColliderComponent::Intersect(const CircleColliderComponent& c) const
{
        Vector2 diff = mOwner->GetPosition() - c.mOwner->GetPosition();
        float distSq = diff.LengthSq();

        float radiiSum = mRadius + c.GetRadius();
        float radiiSumSq = radiiSum * radiiSum;

        return distSq <= radiiSumSq;
}

void CircleColliderComponent::DebugDraw(Renderer *renderer)
{
        renderer->Draw(mOwner->GetModelMatrix(), mDrawArray, Vector3(0.0f, 1.0f, 0.0f)); // Green
}