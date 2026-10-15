//
// Created by Lucas N. Ferreira on 06/11/25.
//

#include "Block.h"
#include "../Game.h"
#include "../Actors/Ship.h"

Block::Block(class Game* game)
        :Actor(game)
{
    // Add mesh component with Cube mesh
    mMeshComponent = new MeshComponent(this);
    auto* mesh = GetGame()->GetRenderer()->GetMesh("../Assets/Cube.gpmesh");
    mMeshComponent->SetMesh(mesh);
}

Block::~Block()
{
}

void Block::OnUpdate(float deltaTime)
{
    // Destroy block if the player has passed it and it's no longer visible on screen.
    Ship* player = GetGame()->GetPlayer();
    if (player) {
        Vector3 pos = GetPosition();
        Vector3 playerPos = player->GetPosition();

        const float margin = static_cast<float>(Game::WALL_SCALE) + 100.0f;
        if (pos.x < playerPos.x - margin) {
            SetState(ActorState::Destroy);
        }
    }
}