//
// Created by Lucas N. Ferreira on 03 Aug 23.
//

#pragma once
#include "Actor.h"

enum class PlayerIdentifier
{
    Player1,
    Player2
};

class Paddle : public Actor
{
public:
    Paddle(class Game* game, int width, int height, PlayerIdentifier player);

    void OnProcessInput(const Uint8* keyState) override;
    void OnUpdate(float deltaTime) override;
    void CreateVertexArray() override;

private:
    float mSpeed;
    int mDirection; // -1 para cima, 1 para baixo, 0 parado
    PlayerIdentifier mPlayer;
};

