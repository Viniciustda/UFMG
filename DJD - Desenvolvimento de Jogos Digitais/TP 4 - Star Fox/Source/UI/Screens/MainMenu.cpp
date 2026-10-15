//
// Created by Lucas N. Ferreira on 06/11/25.
//

#include "MainMenu.h"
#include "../../Game.h"

MainMenu::MainMenu(class Game* game, const std::string& fontName)
        :UIScreen(game, fontName)
{
    // Add game logo
    AddImage("../Assets/Logo.png", Vector2(0.0f, 100.0f), 0.65f);

    // Add start button
    auto startButton = AddButton("New Game", [this]() {
        Close();
        mGame->SetScene(GameScene::Level1);
    }, Vector2(0.0f, -50.0f), 0.65f);
    startButton->SetBackgroundColor(Vector4(0.0f, 0.0f, 1.0f, 1.0f));
    startButton->SetTextColor(Vector3(1.0f, 1.0f, 1.0f));

    // Add quit button
    auto quitButton = AddButton("Exit", [this]() {
        mGame->Quit();
    }, Vector2(0.0f, -100.0f), 0.65f);
    quitButton->SetBackgroundColor(Vector4(0.0f, 0.0f, 1.0f, 1.0f));
    quitButton->SetTextColor(Vector3(1.0f, 1.0f, 1.0f));
}

void MainMenu::HandleKeyPress(int key)
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