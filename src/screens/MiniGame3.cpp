// === MiniGame3.cpp - Jeu de Snake ===
#include "MiniGame3.hpp"
#include "raylib.h"
#include <deque>
#include <cstdlib>
#include <ctime>

const int TILE_SIZE = 20;
const int GRID_WIDTH = 40;   // 800 / 20
const int GRID_HEIGHT = 30;  // 600 / 20

enum Direction { UP, DOWN, LEFT, RIGHT };

struct SnakePart {
    int x, y;
};

static std::deque<SnakePart> snake;
static Direction dir = RIGHT;
static SnakePart apple;
static float timer = 0.0f;
static const float UPDATE_INTERVAL = 0.15f;
static bool gameOver = false;
static int score = 0;

static Texture2D snakeHeadUp;
static Texture2D snakeHeadDown;
static Texture2D snakeHeadLeft;
static Texture2D snakeHeadRight;
static Texture2D snakeBodyUp;
static Texture2D snakeBodyDown;
static Texture2D snakeBodyLeft;
static Texture2D snakeBodyRight;
static Texture2D snakeTailUp;
static Texture2D snakeTailDown;
static Texture2D snakeTailLeft;
static Texture2D snakeTailRight;
static Texture2D appleTexture;

void InitMiniGame3() {
    snakeHeadUp = LoadTexture("assets/snake/snake1_up.png");
    snakeHeadDown = LoadTexture("assets/snake/snake1_down.png");
    snakeHeadLeft = LoadTexture("assets/snake/snake1_left.png");
    snakeHeadRight = LoadTexture("assets/snake/snake1_right.png");

    snakeBodyUp = LoadTexture("assets/snake/snake2_up.png");
    snakeBodyDown = LoadTexture("assets/snake/snake2_down.png");
    snakeBodyLeft = LoadTexture("assets/snake/snake2_left.png");
    snakeBodyRight = LoadTexture("assets/snake/snake2_right.png");

    snakeTailUp = LoadTexture("assets/snake/snake3_up.png");
    snakeTailDown = LoadTexture("assets/snake/snake3_down.png");
    snakeTailLeft = LoadTexture("assets/snake/snake3_left.png");
    snakeTailRight = LoadTexture("assets/snake/snake3_right.png");

    appleTexture = LoadTexture("assets/snake/apple.png");

    snake.clear();
    snake.push_back({ 10, 10 });
    snake.push_back({ 9, 10 });
    snake.push_back({ 8, 10 });
    dir = RIGHT;
    timer = 0.0f;
    gameOver = false;
    score = 0;
    std::srand((unsigned)time(nullptr));
    apple = { rand() % GRID_WIDTH, rand() % GRID_HEIGHT };
}

void UpdateMiniGame3(GameState& gameState) {
    if (IsKeyPressed(KEY_BACKSPACE)) {
        gameState = GameState::MENU;
        return;
    }

    if (gameOver) {
        if (IsKeyPressed(KEY_SPACE)) InitMiniGame3();
        return;
    }

    if (IsKeyPressed(KEY_UP) && dir != DOWN) dir = UP;
    if (IsKeyPressed(KEY_DOWN) && dir != UP) dir = DOWN;
    if (IsKeyPressed(KEY_LEFT) && dir != RIGHT) dir = LEFT;
    if (IsKeyPressed(KEY_RIGHT) && dir != LEFT) dir = RIGHT;

    timer += GetFrameTime();
    if (timer >= UPDATE_INTERVAL) {
        timer = 0.0f;

        SnakePart head = snake.front();
        switch (dir) {
            case UP: head.y--; break;
            case DOWN: head.y++; break;
            case LEFT: head.x--; break;
            case RIGHT: head.x++; break;
        }

        if (head.x < 0 || head.x >= GRID_WIDTH || head.y < 0 || head.y >= GRID_HEIGHT) {
            gameOver = true;
            return;
        }
        for (auto& part : snake) {
            if (head.x == part.x && head.y == part.y) {
                gameOver = true;
                return;
            }
        }

        snake.push_front(head);

        if (head.x == apple.x && head.y == apple.y) {
            apple = { rand() % GRID_WIDTH, rand() % GRID_HEIGHT };
            score++;
        } else {
            snake.pop_back();
        }
    }
}

void DrawMiniGame3() {
    ClearBackground(RAYWHITE);


    Color green1 = (Color){ 170, 215, 81, 255 };
    Color green2 = (Color){ 162, 209, 73, 255 };

    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            Color color = ((x + y) % 2 == 0) ? green1 : green2;
            DrawRectangle(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE, color);
        }
    Rectangle appleSrc = { 0, 0, (float)appleTexture.width, (float)appleTexture.height };
    Rectangle appleDest = { (float)(apple.x * TILE_SIZE), (float)(apple.y * TILE_SIZE), (float)TILE_SIZE, (float)TILE_SIZE };
    Vector2 origin = { 0, 0 };
    DrawTexturePro(appleTexture, appleSrc, appleDest, origin, 0.0f, WHITE);

    DrawText(TextFormat("Score: %d", score), 10, 10, 20, DARKGRAY);

    for (int i = 0; i < (int)snake.size(); i++) {
        SnakePart& part = snake[i];
        Texture2D texture = snakeBodyRight; // default

        if (i == 0) {
            SnakePart& neck = snake[1];
            if (part.x > neck.x) texture = snakeHeadRight;
            else if (part.x < neck.x) texture = snakeHeadLeft;
            else if (part.y > neck.y) texture = snakeHeadDown;
            else if (part.y < neck.y) texture = snakeHeadUp;
        } else if (i == (int)snake.size() - 1) {
            SnakePart& beforeTail = snake[i - 1];
            if (part.x > beforeTail.x) texture = snakeTailLeft;
            else if (part.x < beforeTail.x) texture = snakeTailRight;
            else if (part.y > beforeTail.y) texture = snakeTailUp;
            else if (part.y < beforeTail.y) texture = snakeTailDown;
        } else {
            SnakePart& prev = snake[i - 1];
            SnakePart& next = snake[i + 1];
            if (prev.x == next.x) {
                texture = snakeBodyUp; // vertical
            } else if (prev.y == next.y) {
                texture = snakeBodyRight; // horizontal
            } else if ((prev.x < part.x && next.y < part.y) || (next.x < part.x && prev.y < part.y)) {
                texture = snakeBodyUp; // turn case (adjust as needed)
            } else {
                texture = snakeBodyDown; // turn case (adjust as needed)
            }
        }

        Rectangle src = { 0, 0, (float)texture.width, (float)texture.height };
        Rectangle dest = {
            (float)(part.x * TILE_SIZE),
            (float)(part.y * TILE_SIZE),
            (float)TILE_SIZE,
            (float)TILE_SIZE
        };

        DrawTexturePro(texture, src, dest, origin, 0.0f, WHITE);
    }

    if (gameOver) {
        DrawText("Game Over - ESPACE pour recommencer", 180, 280, 20, MAROON);
    }
}
}

void UnloadMiniGame3() {
    UnloadTexture(snakeHeadUp);
    UnloadTexture(snakeHeadDown);
    UnloadTexture(snakeHeadLeft);
    UnloadTexture(snakeHeadRight);
    UnloadTexture(snakeBodyUp);
    UnloadTexture(snakeBodyDown);
    UnloadTexture(snakeBodyLeft);
    UnloadTexture(snakeBodyRight);
    UnloadTexture(snakeTailUp);
    UnloadTexture(snakeTailDown);
    UnloadTexture(snakeTailLeft);
    UnloadTexture(snakeTailRight);
    UnloadTexture(appleTexture);
}
