// MiniGame1.cpp
#include "MiniGame6.hpp"
#include "raylib.h"

static Texture2D terrain;

void InitMiniGame6() {
    terrain = LoadTexture("assets/MiniJeu1.png");
}

void UpdateMiniGame6(GameState& gameState) {
    if (IsKeyPressed(KEY_BACKSPACE)) {
        InitMiniGame6();
        gameState = GameState::MENU;
        return;
    }
}

void DrawMiniGame6() {
    ClearBackground(RAYWHITE);

    Rectangle src = { 0, 0, (float)terrain.width, (float)terrain.height };
    Rectangle dest = { 0, 0, 800, 600 };
    Vector2 origin = { 0, 0 };
    DrawTexturePro(terrain, src, dest, origin, 0.0f, WHITE);
}

void UnloadMiniGame6() {
    UnloadTexture(terrain);
}