//
// Created by Lucas N. Ferreira on 30/09/23.
//

#include "Spawner.h"
#include "../Game.h"
#include "Mario.h"
#include "Goomba.h"

Spawner::Spawner(Game* game, float spawnDistance)
        :Actor(game)
        ,mSpawnDistance(spawnDistance)
{

}

void Spawner::OnUpdate(float deltaTime)
{
    // Check distance to player to spawn enemy
    const Mario* player = GetGame()->GetPlayer();
    if (player) {
        float dist = fabs(player->GetPosition().x - mPosition.x);

        if (dist < mSpawnDistance) {
            auto* goomba = new Goomba(GetGame(), -60.0f, 0.5f);
            goomba->SetPosition(mPosition);
            mState = ActorState::Destroy;
        }
    }
}