#include "MiniGame5.hpp"
#include "raylib.h"

static Texture2D terrain;

void InitMiniGame5() {
    terrain = LoadTexture("assets/MiniJeu1.png");
}

void UpdateMiniGame5(GameState& gameState) {
    if (IsKeyPressed(KEY_BACKSPACE)) {
        InitMiniGame5();
        gameState = GameState::MENU;
        return;
    }
}

void DrawMiniGame5() {
    ClearBackground(RAYWHITE);

    Rectangle src = { 0, 0, (float)terrain.width, (float)terrain.height };
    Rectangle dest = { 0, 0, 800, 600 };
    Vector2 origin = { 0, 0 };
    DrawTexturePro(terrain, src, dest, origin, 0.0f, WHITE);
    DrawText("À venir. Appuyez sur retour pour revenir au menu", 200, 300, 20, GRAY);
}

void UnloadMiniGame5() {
    UnloadTexture(terrain);
}