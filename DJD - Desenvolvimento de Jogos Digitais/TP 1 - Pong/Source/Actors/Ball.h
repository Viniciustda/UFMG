//
// Created by Lucas N. Ferreira on 03 Aug 23.
//

#pragma once
#include "Actor.h"

class Ball : public Actor
{
public:
    Ball(struct Game *game, int width, int height);
    void OnUpdate(float deltaTime) override;
    void CreateVertexArray() override;
    void Reset();
    
private:
    Vector2 mVelocity;
    float mSpeed;
};
