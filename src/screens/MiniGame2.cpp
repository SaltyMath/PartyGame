#include "MiniGame2.hpp"
#include "raylib.h"
#include <vector>
#include <algorithm>
#include <ctime>
#include <string>
#include <random>

const int ROWS = 4;
const int COLS = 6;
const int CARD_SIZE = 80;
const int CARD_PADDING = 10;
const int TOTAL_PAIRS = (ROWS * COLS) / 2;

struct Card {
    int value;
    bool revealed = false;
    bool matched = false;
};

static std::vector<Card> cards;
static Texture2D cardTextures[TOTAL_PAIRS];
static Texture2D cardBack;

static int firstSelection = -1;
static int secondSelection = -1;
static float revealTimer = 0.0f;
static const float REVEAL_DELAY = 1.0f;

static int remainingTime = 60;
static float timerAccumulator = 0.0f;
static bool gameWon = false;
static bool gameOver = false;

void InitMiniGame2() {
    cards.clear();
    for (int i = 0; i < TOTAL_PAIRS; ++i) {
        cards.push_back({ i });
        cards.push_back({ i });
    }
    std::srand((unsigned int)time(nullptr));
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(cards.begin(), cards.end(), g);

    for (int i = 0; i < TOTAL_PAIRS; ++i) {
        std::string path = "assets/memory/img" + std::to_string(i) + ".png";
        cardTextures[i] = LoadTexture(path.c_str());
        if (cardTextures[i].id <= 0) {
            TraceLog(LOG_ERROR, TextFormat("Erreur chargement %s", path.c_str()));
        }
    }
    
    cardBack = LoadTexture("assets/memory/back.png");
    if (cardBack.id <= 0) {
        TraceLog(LOG_ERROR, "Erreur de chargement de back.png");
    }

    firstSelection = -1;
    secondSelection = -1;
    revealTimer = 0.0f;
    remainingTime = 60;
    timerAccumulator = 0.0f;
    gameWon = false;
    gameOver = false;
}

void UpdateMiniGame2(GameState& gameState) {
    if (IsKeyPressed(KEY_BACKSPACE)) {
        InitMiniGame2();
        gameState = GameState::MENU;
        return;
    }

    if ((gameWon || gameOver) && IsKeyPressed(KEY_SPACE)) {
        InitMiniGame2();
        return;
    }

    // Timer ne descend que si la partie est en cours
    if (!gameWon && !gameOver) {
        timerAccumulator += GetFrameTime();
        if (timerAccumulator >= 1.0f) {
            timerAccumulator = 0.0f;
            remainingTime--;
            if (remainingTime <= 0) {
                remainingTime = 0;
                gameOver = true;
            }
        }
    }

    // Gestion du délai entre deux sélections
    if ((gameWon || gameOver) || revealTimer > 0.0f) {
        if (revealTimer > 0.0f) {
            revealTimer -= GetFrameTime();
            if (revealTimer <= 0.0f) {
                if (cards[firstSelection].value != cards[secondSelection].value) {
                    cards[firstSelection].revealed = false;
                    cards[secondSelection].revealed = false;
                } else {
                    cards[firstSelection].matched = true;
                    cards[secondSelection].matched = true;
                }
                firstSelection = -1;
                secondSelection = -1;
            }
        }
        return;
    }

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 mouse = GetMousePosition();

        for (int i = 0; i < (int)cards.size(); ++i) {
            int row = i / COLS;
            int col = i % COLS;
            Rectangle bounds = {
                (float)135 + col * (CARD_SIZE + CARD_PADDING),
                (float)145 + row * (CARD_SIZE + CARD_PADDING),
                (float)CARD_SIZE, (float)CARD_SIZE
            };

            if (!cards[i].revealed && !cards[i].matched && CheckCollisionPointRec(mouse, bounds)) {
                cards[i].revealed = true;
                if (firstSelection == -1) firstSelection = i;
                else if (secondSelection == -1 && i != firstSelection) {
                    secondSelection = i;
                    revealTimer = REVEAL_DELAY;
                }
                break;
            }
        }
    }

    // Vérification victoire
    gameWon = std::all_of(cards.begin(), cards.end(), [](const Card& c) { return c.matched; });
}

void DrawMiniGame2() {
    ClearBackground(RAYWHITE);
    DrawRectangle(0, 0, 800, 40, BLACK);

    for (int i = 0; i < (int)cards.size(); ++i) {
        int row = i / COLS;
        int col = i % COLS;
        Rectangle rect = {
            (float)135 + col * (CARD_SIZE + CARD_PADDING),
            (float)145 + row * (CARD_SIZE + CARD_PADDING),
            (float)CARD_SIZE, (float)CARD_SIZE
        };

        if (cards[i].revealed || cards[i].matched) {
            DrawTexturePro(cardTextures[cards[i].value],
                { 0, 0, (float)cardTextures[cards[i].value].width, (float)cardTextures[cards[i].value].height },
                rect, { 0, 0 }, 0.0f, WHITE);
        } else {
            DrawTexturePro(cardBack,
                { 0, 0, (float)cardBack.width, (float)cardBack.height },
                rect, { 0, 0 }, 0.0f, WHITE);
        }

        DrawRectangleLinesEx(rect, 2, BLACK);
    }

    DrawText(TextFormat("Temps restant: %d", remainingTime), 50, 10, 20, RED);

    if (gameWon) {
        DrawText("Victoire !!", 100, 550, 20, DARKGREEN);
    }
    if (gameOver) {
        DrawText("Temps écoulé...", 100, 550, 20, RED);
    }
}

void UnloadMiniGame2() {
    for (int i = 0; i < TOTAL_PAIRS; ++i) {
        UnloadTexture(cardTextures[i]);
    }
    UnloadTexture(cardBack);
}
