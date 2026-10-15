//
// Created by Lucas N. Ferreira on 03/11/25.
//

#include "BlockObstacle.h"
#include "../Game.h"

BlockObstacle::BlockObstacle(class Game* game, const bool isExploding)
        :Block(game)
        ,mIsExploding(isExploding)
{
    mColliderComponent = new AABBColliderComponent(this, 1.0f, 1.0f, 1.0f, Vector3::Zero, ColliderLayer::Block, true);
    mGame->AddObstacle(this);
}

BlockObstacle::~BlockObstacle()
{
    mGame->RemoveObstacle(this);
}

void BlockObstacle::Explode()
{
    // Prevent re-triggering explosions
    if (GetState() == ActorState::Destroy) {
        return;
    }

    // Play explosion sound
    mGame->GetAudio()->PlaySound("BlockExplode.wav");

    // Destroy this obstacle
    SetState(ActorState::Destroy);

    // If this block is explosive, trigger explosions on its neighbors
    if (mIsExploding) {
        // Get all obstacles to check for neighbors
        const auto& obstacles = mGame->GetObstacles();
        for (auto obstacle : obstacles) {
            if (obstacle != this) {
                // Check distance to the other obstacle
                float distance = Vector3::Distance(GetPosition(), obstacle->GetPosition());
                if (distance <= 50.0f) {
                    // Trigger explosion on the neighbor
                    obstacle->Explode();
                }
            }
        }
    }
}