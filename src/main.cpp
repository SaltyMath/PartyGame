#include "raylib.h"
#include "screens/TitleScreen.hpp"
#include "screens/MenuScreen.hpp"
#include "screens/MiniGame1.hpp"
#include "screens/MiniGame2.hpp"
#include "screens/MiniGame3.hpp"
#include "screens/MiniGame4.hpp"
#include "screens/MiniGame5.hpp"
#include "screens/MiniGame6.hpp"
#include "GameState.hpp"

// Ajoute tes fonctions Commandes ici si ce n’est pas dans un header
void UpdateCommandesTir(GameState& state);
void UpdateCommandesMemory(GameState& state);
void UpdateCommandesSnake(GameState& state);
void UpdateCommandesFE(GameState& state);
void UpdateCommandesTetris(GameState& state);

float menuCooldown = 0.0f;

int main() {
    InitWindow(800, 600, "The Legend of Mathieu Party Final 3 the hedgehog 2077 SOLID");
    InitAudioDevice();

    InitTitleScreen();
    InitMenuScreen();
    InitMiniGame1();
    InitMiniGame2();
    InitMiniGame3();
    InitMiniGame4();
    InitMiniGame5();
    InitMiniGame6();

    SetTargetFPS(60);

    GameState state = GameState::TITLE;

    while (!WindowShouldClose()) {

        // LOGIQUE
        switch (state) {
            case GameState::TITLE:
                UpdateTitleScreen(state);
                break;
            case GameState::MENU:
                UpdateMenuScreen(state);
                break;
            case GameState::COMMANDES_TIR:
                UpdateCommandesTir(state);
                break;
            case GameState::COMMANDES_MEMORY:
                UpdateCommandesMemory(state);
                break;
            case GameState::COMMANDES_SNAKE:
                UpdateCommandesSnake(state);
                break;
            case GameState::COMMANDES_FE:
                UpdateCommandesFE(state);
                break;
            case GameState::COMMANDES_TETRIS:
                UpdateCommandesTetris(state);
            case GameState::MINIGAME1:
                UpdateMiniGame1(state);
                break;
            case GameState::MINIGAME2:
                UpdateMiniGame2(state);
                break;
            case GameState::MINIGAME3:
                UpdateMiniGame3(state);
                break;
            case GameState::MINIGAME4:
                UpdateMiniGame4(state);
                break;
            case GameState::MINIGAME5:
                UpdateMiniGame5(state);
                break;
            case GameState::MINIGAME6:
                UpdateMiniGame6(state);
                break;
            case GameState::EXIT:
                CloseWindow();
                return 0;
        }

        // AFFICHAGE
        BeginDrawing();
        ClearBackground(RAYWHITE);

        switch (state) {
            case GameState::TITLE:
                DrawTitleScreen();
                break;
            case GameState::MENU:
                DrawMenuScreen();
                break;
            case GameState::MINIGAME1:
                DrawMiniGame1();
                break;
            case GameState::MINIGAME2:
                DrawMiniGame2();
                break;
            case GameState::MINIGAME3:
                DrawMiniGame3();
                break;
            case GameState::MINIGAME4:
                DrawMiniGame4();
                break;
            case GameState::MINIGAME5:
                DrawMiniGame5();
                break;
            case GameState::MINIGAME6:
                DrawMiniGame6();
                break;
            // PAS de Draw pour les GameStates de commandes car ils dessinent déjà dans Update
            default:
                break;
        }

        EndDrawing();
    }

    // NETTOYAGE
    UnloadTitleScreen();
    UnloadMenuScreen();
    UnloadMiniGame1();
    UnloadMiniGame2();
    UnloadMiniGame3();
    UnloadMiniGame4();
    UnloadMiniGame5();
    UnloadMiniGame6();

    CloseAudioDevice();
    CloseWindow();
    return 0;
}
