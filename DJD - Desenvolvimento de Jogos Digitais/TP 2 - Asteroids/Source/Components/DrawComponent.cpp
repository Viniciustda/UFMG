//
// Created by Lucas N. Ferreira on 03/08/23.
//

#include "DrawComponent.h"
#include "CircleColliderComponent.h"
#include "../Game.h"

DrawComponent::DrawComponent(class Actor* owner, std::vector<Vector2> &vertices, int drawOrder)
    :Component(owner)
    ,mDrawOrder(drawOrder)
    ,mIsVisible(true)
    ,mColor(1.0f, 1.0f, 1.0f) // Default to white
{
    mOwner->GetGame()->AddDrawable(this);

    const unsigned int numVerts = vertices.size();

    std::vector<float> vertexBuffer;
    vertexBuffer.reserve(numVerts * 2);
    for(const auto& vert : vertices) {
        vertexBuffer.emplace_back(vert.x);
        vertexBuffer.emplace_back(vert.y);
    }

    std::vector<unsigned int> indexBuffer;
    for (unsigned int i = 0; i < numVerts; i++) {
        indexBuffer.emplace_back(i);
        indexBuffer.emplace_back((i + 1) % numVerts);
    }

    mDrawArray = new VertexArray(vertexBuffer.data(), numVerts, indexBuffer.data(), indexBuffer.size());
}

DrawComponent::~DrawComponent()
{
    mOwner->GetGame()->RemoveDrawable(this);
    delete mDrawArray;
    mDrawArray = nullptr;
}

void DrawComponent::Draw(Renderer *renderer)
{
    if (mIsVisible) {
        renderer->Draw(mOwner->GetModelMatrix(), mDrawArray, mColor);
    }
}
