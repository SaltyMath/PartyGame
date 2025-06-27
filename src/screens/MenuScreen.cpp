#include "MenuScreen.hpp"
#include "MiniGame1.hpp"
#include "MiniGame2.hpp"
#include "MiniGame3.hpp"
#include "MiniGame4.hpp"
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
    buttons[0].targetState = GameState::COMMANDES_TIR;

    buttons[1].bounds = { 265, 150, 100, 100 };
    buttons[1].texture = LoadTexture("assets/memory/back.png");
    buttons[1].targetState = GameState::COMMANDES_MEMORY;

    buttons[2].bounds = { 440, 150, 100, 100 };
    buttons[2].texture = LoadTexture("assets/snake/snakelogo.png");
    buttons[2].targetState = GameState::COMMANDES_SNAKE;

    buttons[3].bounds = { 600, 150, 100, 100 };
    buttons[3].texture = LoadTexture("assets/FE/felogo.png");
    buttons[3].targetState = GameState::COMMANDES_FE;

    // buttons[4].bounds = { 100, 300, 100, 100 };
    // buttons[4].texture = LoadTexture("assets/foot/MiniJeu1.png");
    // buttons[4].targetState = GameState::MINIGAME5;

    // buttons[5].bounds = { 265, 300, 100, 100 };
    // buttons[5].texture = LoadTexture("assets/foot/MiniJeu1.png");
    // buttons[5].targetState = GameState::MINIGAME6;

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
        // Retour à la fenêtre précédente si retour est utilisé dans un jeu
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

void UpdateCommandesTir(GameState& gameState) {
    ClearBackground(RAYWHITE);
    DrawText("Commandes du Mini-Jeu : Tir au But", 100, 80, 28, BLACK);
    DrawText("Joueur 1 (Tireur) : A / S / D / F", 100, 140, 20, DARKGRAY);
    DrawText("Joueur 2 (Gardien) : Q / W / E / R", 100, 170, 20, DARKGRAY);
    DrawText("Espace : Recommencer après le tir", 100, 200, 20, DARKGRAY);
    DrawText("Backspace : Revenir au menu", 100, 230, 20, DARKGRAY);
    DrawText("Si les deux joueurs appuient sur des touches équivalentes, c'est un arrêt. Si elles diffèrent, c'est un but.", 100, 260, 5, DARKGRAY);

    Rectangle bouton = { 100, 300, 200, 40 };
    DrawRectangleRec(bouton, LIGHTGRAY);
    DrawText("Commencer", 120, 310, 20, BLACK);

    if (CheckCollisionPointRec(GetMousePosition(), bouton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        InitMiniGame1();
        gameState = GameState::MINIGAME1;
    }

    if (IsKeyPressed(KEY_BACKSPACE)) {
        gameState = GameState::MENU;
    }
}
void UpdateCommandesMemory(GameState& gameState) {
    ClearBackground(RAYWHITE);
    DrawText("Commandes du Mini-Jeu : Memory", 100, 80, 28, BLACK);
    DrawText("Clique sur deux cartes pour les retourner", 100, 140, 20, DARKGRAY);
    DrawText("Si elles sont identiques, elles restent visibles", 100, 170, 20, DARKGRAY);
    DrawText("Sinon, elles se retournent après un court délai", 100, 200, 20, DARKGRAY);
    DrawText("Trouve toutes les paires pour gagner !", 100, 230, 20, DARKGRAY);
    DrawText("Backspace : Revenir au menu", 100, 260, 20, DARKGRAY);

    Rectangle bouton = { 100, 320, 200, 40 };
    DrawRectangleRec(bouton, LIGHTGRAY);
    DrawText("Commencer", 120, 330, 20, BLACK);

    if (CheckCollisionPointRec(GetMousePosition(), bouton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        InitMiniGame2();
        gameState = GameState::MINIGAME2;
    }

    if (IsKeyPressed(KEY_BACKSPACE)) {
        gameState = GameState::MENU;
    }
}
void UpdateCommandesSnake(GameState& gameState) {
    ClearBackground(RAYWHITE);
    DrawText("Commandes du Mini-Jeu : Snake", 100, 80, 28, BLACK);
    DrawText("Flèches directionnelles : déplace le serpent", 100, 140, 20, DARKGRAY);
    DrawText("Mange les pommes pour grandir", 100, 170, 20, DARKGRAY);
    DrawText("Évite les murs et ton propre corps, ne tourne pas trop vite", 100, 200, 20, DARKGRAY);
    DrawText("Backspace : Revenir au menu", 100, 230, 20, DARKGRAY);

    Rectangle bouton = { 100, 300, 200, 40 };
    DrawRectangleRec(bouton, LIGHTGRAY);
    DrawText("Commencer", 120, 310, 20, BLACK);

    if (CheckCollisionPointRec(GetMousePosition(), bouton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        InitMiniGame3();
        gameState = GameState::MINIGAME3;
    }

    if (IsKeyPressed(KEY_BACKSPACE)) {
        gameState = GameState::MENU;
    }
}
void UpdateCommandesFE(GameState& gameState) {
    ClearBackground(RAYWHITE);
    DrawText("Commandes du Mini-Jeu : Combat au tour par tour", 100, 80, 28, BLACK);
    DrawText("Clic : Sélectionne une unité ou une case", 100, 140, 20, DARKGRAY);
    DrawText("WASD : Déplacement de la sélection", 100, 170, 20, DARKGRAY);
    DrawText("Espace : Confirme le déplacement ou l'attaque", 100, 200, 20, DARKGRAY);
    DrawText("Gagne en éliminant toutes les unités ennemies !", 100, 230, 20, DARKGRAY);
    DrawText("Backspace : Revenir au menu", 100, 260, 20, DARKGRAY);

    Rectangle bouton = { 100, 320, 200, 40 };
    DrawRectangleRec(bouton, LIGHTGRAY);
    DrawText("Commencer", 120, 330, 20, BLACK);

    if (CheckCollisionPointRec(GetMousePosition(), bouton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        InitMiniGame4();
        gameState = GameState::MINIGAME4;
    }

    if (IsKeyPressed(KEY_BACKSPACE)) {
        gameState = GameState::MENU;
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
