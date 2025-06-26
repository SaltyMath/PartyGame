// MiniGame1.cpp
#include "MiniGameXX.hpp"
#include "raylib.h"

static Texture2D terrain;

void InitMiniGameX() {
    terrain = LoadTexture("assets/MiniJeu1.png");
}

void UpdateMiniGameX(GameState& gameState) {
    if (IsKeyPressed(KEY_BACKSPACE)) {
        InitMiniGameX();
        gameState = GameState::MENU;
        return;
    }
}

void DrawMiniGameX() {
    ClearBackground(RAYWHITE);

    Rectangle src = { 0, 0, (float)terrain.width, (float)terrain.height };
    Rectangle dest = { 0, 0, 800, 600 };
    Vector2 origin = { 0, 0 };
    DrawTexturePro(terrain, src, dest, origin, 0.0f, WHITE);
}

void UnloadMiniGameX() {
    UnloadTexture(terrain);
}