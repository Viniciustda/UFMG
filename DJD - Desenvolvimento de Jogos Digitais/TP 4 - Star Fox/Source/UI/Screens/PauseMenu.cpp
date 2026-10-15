//
// Created by Vinicius Trindade on 12/11/25.
//

#include "PauseMenu.h"
#include "../../Game.h"
#include "../../Actors/Ship.h"

PauseMenu::PauseMenu(class Game* game, const std::string& fontName)
        :UIScreen(game, fontName)
{
        auto* pauseText = AddText("Paused", Vector2(0.0f, 50.0f), 1.5f);
        pauseText->SetBackgroundColor(Vector4(0.0f, 0.0f, 1.0f, 0.5f)); 
        pauseText->SetTextColor(Vector3(1.0f, 1.0f, 1.0f));

        auto continueButton = AddButton("Continue", [this]() {
            Close();
            mGame->SetPaused(false);
        }, Vector2(0.0f, -50.0f), 0.65f);
        continueButton->SetBackgroundColor(Vector4(0.0f, 0.0f, 1.0f, 1.0f));
        continueButton->SetTextColor(Vector3(1.0f, 1.0f, 1.0f));

        auto exitButton = AddButton("Exit", [this]() {
            Close();
            mGame->SetPaused(false);
            mGame->SetScene(GameScene::MainMenu);
        }, Vector2(0.0f, -100.0f), 0.65f);
        exitButton->SetBackgroundColor(Vector4(0.0f, 0.0f, 1.0f, 1.0f));
        exitButton->SetTextColor(Vector3(1.0f, 1.0f, 1.0f));
}

void PauseMenu::HandleKeyPress(int key)
{
    UIScreen::HandleKeyPress(key);

    if (key == SDLK_DOWN) {
        mButtons[mSelectedButtonIndex]->SetHighlighted(false);
        mSelectedButtonIndex = (mSelectedButtonIndex + 1) % mButtons.size();
        mButtons[mSelectedButtonIndex]->SetHighlighted(true);
    }
    else if (key == SDLK_UP) {
        mButtons[mSelectedButtonIndex]->SetHighlighted(false);
        mSelectedButtonIndex--;
        if (mSelectedButtonIndex < 0) {
            mSelectedButtonIndex = mButtons.size() - 1;
        }
        mButtons[mSelectedButtonIndex]->SetHighlighted(true);
    }
    else if (key == SDLK_RETURN) { 
        if (mSelectedButtonIndex != -1) {
            mButtons[mSelectedButtonIndex]->OnClick();
        }
    }
}