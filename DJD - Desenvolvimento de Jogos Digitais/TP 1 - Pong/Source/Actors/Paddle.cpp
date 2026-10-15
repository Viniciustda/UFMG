//
// Created by Lucas N. Ferreira on 03 Aug 23.
//

#include "Paddle.h"
#include "../Game.h"

Paddle::Paddle(Game* game, int width, int height, PlayerIdentifier player)
        :Actor(game, width, height)
        , mSpeed(500.0f)
        , mDirection(0)
        , mPlayer(player)
{
    // Define a cor da raquete
    if (player == PlayerIdentifier::Player1) {
        SetColor(Vector3(1.0f, 0.0f, 0.0f)); // Vermelho
    }
    else {
        SetColor(Vector3(1.0f, 1.0f, 0.0f)); // Amarelo
    }

    // Cria o vertex array
    CreateVertexArray();

    Vector2 pos = GetPosition();
    pos.y = (static_cast<float>(Game::WINDOW_HEIGHT)) / 2.0f;
    if (player == PlayerIdentifier::Player1) {
        pos.x = 30.0f;
    }
    else {
        pos.x = static_cast<float>(Game::WINDOW_WIDTH) - 30.0f;
    }
    SetPosition(pos);
}

void Paddle::OnProcessInput(const uint8_t* keyState)
{
    // Reseta a direção
    mDirection = 0;

    if (mPlayer == PlayerIdentifier::Player1)
    {
        // W para cima, S para baixo
        if (keyState[SDL_SCANCODE_W]) {
            mDirection = -1;
        }
        else if (keyState[SDL_SCANCODE_S]) {
            mDirection = 1;
        }
    }
    else // Player2
    {
        // Seta para cima, Seta para baixo
        if (keyState[SDL_SCANCODE_UP]) {
            mDirection = -1;
        }
        else if (keyState[SDL_SCANCODE_DOWN]) {
            mDirection = 1;
        }
    }
}

void Paddle::OnUpdate(float deltaTime)
{
    if (mDirection == 0) return;

    // Atualiza a posição
    Vector2 pos = GetPosition();
    pos.y += mDirection * mSpeed * deltaTime;

    // Define os limites da tela
    float halfHeight = static_cast<float>(GetHeight()) / 2.0f;
    float minY = halfHeight;
    float maxY = static_cast<float>(Game::WINDOW_HEIGHT) - halfHeight;

    // Mantém a raquete dentro dos limites
    pos.y = Math::Clamp(pos.y, minY, maxY);

    // Aplica a nova posição
    SetPosition(pos);
}

void Paddle::CreateVertexArray()
{
    // Vértices do retângulo (x, y) em sentido horário
    const float verts[] = {
        -0.5f, 0.5f,   // 0: esquerda-superior
        0.5f,  0.5f,   // 1: direita-superior
        0.5f, -0.5f,   // 2: direita-inferior
        -0.5f, -0.5f    // 3: esquerda-inferior
    };

    // Índices para dois triângulos
    const unsigned int indices[] = {
        0, 1, 2,   // Triângulo 1
        0, 2, 3    // Triângulo 2
    };

    // Cria VertexArray e guarda em mVertexArray
    mVertexArray = new VertexArray(verts, 4, indices, 6);
}