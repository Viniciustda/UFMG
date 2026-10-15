//
// Created by Lucas N. Ferreira on 03 Aug 23.
//

#include "Ball.h"
#include "Paddle.h"
#include "../Game.h"
#include "../Math.h"

Ball::Ball(struct Game *game, int width, int height)
      :Actor(game, width, height)
      , mSpeed(300.0f)
{
    // Define a cor da bola
    SetColor(Vector3(0.0f, 1.0f, 1.0f)); // Ciano

    // Cria o vertex array
    CreateVertexArray();
    
    // A bola começa indo para a direita
    mVelocity = Vector2(mSpeed, 0.0f);

    // Posição inicial
    Vector2 pos = GetPosition();
    pos.x = (static_cast<float>(Game::WINDOW_WIDTH)) / 2.0f;
    pos.y = (static_cast<float>(Game::WINDOW_HEIGHT)) / 2.0f;
    SetPosition(pos);

}

void Ball::OnUpdate(float deltaTime)
{
    // Move a bolinha
    Vector2 pos = GetPosition();
    pos += mVelocity * deltaTime;

    // Define os limites da tela
    float minY = GetHeight() * 0.5f;
    float maxY = static_cast<float>(Game::WINDOW_HEIGHT) - GetHeight() * 0.5f;

    // Colisão em cima e embaixo: reflete Y
    if (pos.y <= minY) {
        pos.y = minY;
        mVelocity.y *= -1;
    }
    else if (pos.y >= maxY) {
        pos.y = maxY;
        mVelocity.y *= -1;
    }

    // Saiu pela direita ou esquerda: reseta a bola
    float minX = 0.0f;
    float maxX = static_cast<float>(Game::WINDOW_WIDTH);
    if (pos.x <= minX || pos.x >= maxX) {
        GetGame()->ResetRound();
        return;
    }

    // Função auxiliar para a lógica de colisão
    auto HandleCollision = [&](Paddle* paddle) {
        Vector2 pPos = paddle->GetPosition();
        
        float bLeft = pos.x - GetWidth() * 0.5f;
        float bRight = pos.x + GetWidth() * 0.5f;
        float bTop = pos.y - GetHeight() * 0.5f;
        float bBottom = pos.y + GetHeight() * 0.5f;

        float pLeft = pPos.x - paddle->GetWidth() * 0.5f;
        float pRight = pPos.x + paddle->GetWidth() * 0.5f;
        float pTop = pPos.y - paddle->GetHeight() * 0.5f;
        float pBottom = pPos.y + paddle->GetHeight() * 0.5f;

        // Verifica se há interseção
        if (!(bRight < pLeft || bLeft > pRight || bBottom < pTop || bTop > pBottom))
        {
            // Ajusta a posição para evitar que a bola fique presa
            if (mVelocity.x < 0.0f) { // Colidindo com a raquete da esquerda
                pos.x = pRight + GetWidth() * 0.5f;
            }
            else { // Colidindo com a raquete da direita
                pos.x = pLeft - GetWidth() * 0.5f;
            }

            // Inverte a velocidade X
            mVelocity.x *= -1.0f;

            // Calcula a nova velocidade Y com base no ponto de impacto
            float diff = pos.y - pPos.y;
            float normalized = diff / (paddle->GetHeight() * 0.5f);
            mVelocity.y = normalized * mSpeed;

            // Aumenta a velocidade da bola
            mSpeed *= 1.1f;

            // Normaliza o vetor de velocidade e aplica a velocidade
            mVelocity.Normalize();
            mVelocity = mVelocity * mSpeed;
        }
    };

    // Colisão com a raquete 1
    Paddle* paddle1 = GetGame()->GetPaddle1();
    if(paddle1 && mVelocity.x < 0.0f) {
        HandleCollision(paddle1);
    }

    // Colisão com a raquete 2
    Paddle* paddle2 = GetGame()->GetPaddle2();
    if(paddle2 && mVelocity.x > 0.0f) {
        HandleCollision(paddle2);
    }

    // Aplica a nova posição
    SetPosition(pos);
}

void Ball::CreateVertexArray()
{
    const int SIDES = 32;
    const float RADIUS = 0.5f;
    std::vector<float> verts;
    // Vértice central
    verts.push_back(0.0f);
    verts.push_back(0.0f);

    // Vértices da borda
    for (int i = 0; i <= SIDES; ++i) {
        float angle = (static_cast<float>(i) / SIDES) * Math::TwoPi;
        verts.push_back(RADIUS * Math::Cos(angle)); // x
        verts.push_back(RADIUS * Math::Sin(angle)); // y
    }

    std::vector<unsigned int> indices;
    // Cria triângulos a partir do centro
    for (int i = 1; i <= SIDES; ++i) {
        indices.push_back(0); // Centro
        indices.push_back(i);
        indices.push_back(i + 1);
    }

    mVertexArray = new VertexArray(verts.data(), SIDES + 2, indices.data(), SIDES * 3);
}

void Ball::Reset()
{
    // Posição inicial
    Vector2 pos = GetPosition();
    pos.x = (static_cast<float>(Game::WINDOW_WIDTH)) / 2.0f;
    pos.y = (static_cast<float>(Game::WINDOW_HEIGHT)) / 2.0f;

    // Reseta a velocidade
    mSpeed = 300.0f;

    // A bola começa indo para a direita
    mVelocity = Vector2(mSpeed, 0.0f);

    SetPosition(pos);
}