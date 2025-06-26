#include "MenuScreen.hpp"
#include "raylib.h"

extern float menuCooldown;

// Définition du nombre de boutons
static const int BUTTON_COUNT = 6;

struct MiniGameButton {
    Rectangle bounds;
    Texture2D texture;
    GameState targetState;
};

static MiniGameButton buttons[BUTTON_COUNT];
static bool initialized = false;
static float clickCooldown = 0.0f;

// Icônes des minijeux
void InitMenuScreen() {
    if (initialized) return;

    buttons[0].bounds = { 100, 150, 100, 100 };
    buttons[0].texture = LoadTexture("assets/foot/ball.png");
    buttons[0].targetState = GameState::MINIGAME1;

    buttons[1].bounds = { 265, 150, 100, 100 };
    buttons[1].texture = LoadTexture("assets/memory/back.png");
    buttons[1].targetState = GameState::MINIGAME2;

    buttons[2].bounds = { 440, 150, 100, 100 };
    buttons[2].texture = LoadTexture("assets/snake/snakelogo.png");
    buttons[2].targetState = GameState::MINIGAME3;

    buttons[3].bounds = { 600, 150, 100, 100 };
    buttons[3].texture = LoadTexture("assets/FE/felogo.png");
    buttons[3].targetState = GameState::MINIGAME4;

    buttons[4].bounds = { 100, 300, 100, 100 };
    buttons[4].texture = LoadTexture("assets/foot/MiniJeu1.png");
    buttons[4].targetState = GameState::MINIGAME5;

    buttons[5].bounds = { 265, 300, 100, 100 };
    buttons[5].texture = LoadTexture("assets/foot/MiniJeu1.png");
    buttons[5].targetState = GameState::MINIGAME6;

    initialized = true;
}
// Nettoyage du menu
void UnloadMenuScreen() {
    if (!initialized) return;

    for (int i = 0; i < BUTTON_COUNT; i++) {
        UnloadTexture(buttons[i].texture);
    }

    initialized = false;
}
// Mise à jour du menu
void UpdateMenuScreen(GameState& state) {

    if (menuCooldown > 0.0f) return;

    Vector2 mouse = GetMousePosition();
        // Retour à la fenêtre précédente si backspace est utilisé dans un jeu
        if (IsKeyPressed(KEY_BACKSPACE)) {
            state = GameState::MENU;
            return;
        }
        for (int i = 0; i < BUTTON_COUNT; i++) {
            if (CheckCollisionPointRec(mouse, buttons[i].bounds)) {
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    state = buttons[i].targetState;
                    clickCooldown = 0.3f;
            }
        }
    }
}

// Affichage du menu
void DrawMenuScreen() {
    ClearBackground(RAYWHITE);

    DrawText("Choisissez un mini-jeu :", 100, 50, 30, BLACK);

    Vector2 mouse = GetMousePosition();

    for (int i = 0; i < BUTTON_COUNT; i++) {
        DrawRectangleRec(buttons[i].bounds, LIGHTGRAY);

        DrawTexturePro(buttons[i].texture,
                       { 0, 0, (float)buttons[i].texture.width, (float)buttons[i].texture.height },
                       buttons[i].bounds,
                       { 0, 0 },
                       0.0f,
                       WHITE);

        if (CheckCollisionPointRec(mouse, buttons[i].bounds)) {
            DrawRectangleLinesEx(buttons[i].bounds, 3, RED);
        }
    }
}
