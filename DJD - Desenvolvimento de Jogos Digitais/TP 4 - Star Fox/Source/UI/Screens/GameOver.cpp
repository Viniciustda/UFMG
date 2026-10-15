//
// Created by Lucas N. Ferreira on 06/11/25.
//

#include "GameOver.h"
#include "../../Game.h"

GameOver::GameOver(class Game* game, const std::string& fontName)
        :UIScreen(game, fontName)
{
        auto* gameOverText = AddText("Game Over!", Vector2(0.0f, 50.0f), 1.5f);
        gameOverText->SetBackgroundColor(Vector4(0.0f, 0.0f, 1.0f, 1.0f)); 
        gameOverText->SetTextColor(Vector3(1.0f, 1.0f, 1.0f));

        auto menuButton = AddButton("Press Enter", [this]() {
            Close();
            mGame->SetScene(GameScene::MainMenu);
        }, Vector2(0.0f, -50.0f), 0.65f);
        menuButton->SetBackgroundColor(Vector4(0.0f, 0.0f, 1.0f, 1.0f));
        menuButton->SetTextColor(Vector3(1.0f, 1.0f, 1.0f));
}

void GameOver::HandleKeyPress(int key)
{
        UIScreen::HandleKeyPress(key);
        
        if (key == SDLK_RETURN) {
                if (mSelectedButtonIndex != -1) {
                        mButtons[mSelectedButtonIndex]->OnClick();
                }
        }
}