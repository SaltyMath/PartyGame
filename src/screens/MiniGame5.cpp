#include "MiniGame5.hpp"
#include "raylib.h"
#include <vector>
#include <cstdlib>
#include <ctime>

static const int GRID_WIDTH = 10;
static const int GRID_HEIGHT = 20;
static const int CELL_SIZE = 30;

static const int GRID_PIXEL_WIDTH = GRID_WIDTH * CELL_SIZE;
static const int GRID_PIXEL_HEIGHT = GRID_HEIGHT * CELL_SIZE;
static const int OFFSET_X = (800 - GRID_PIXEL_WIDTH) / 2;
static const int OFFSET_Y = (600 - GRID_PIXEL_HEIGHT) / 2;

static const int GAME_OVER_LINE = 2;

enum PieceType { I, O, T, L, J, S, Z };

struct PieceDefinition {
    int rotations[4][4][4];
    int numRotations;
    Color color;
    PieceType type;
};

static Texture2D blockTextures[7];
static bool textureAvailable[7];
static PieceDefinition pieces[7];

static int grid[GRID_HEIGHT][GRID_WIDTH] = { 0 };
static PieceType currentPieceType;
static int currentRotation = 0;
static Vector2 blockPos;
static float fallTimer = 0.0f;
static float fallDelay = 0.5f;
static float fastFallDelay = 0.05f;
static bool isGameOver = false;
static int score = 0;

void LoadBlockTextures() {
    const char* texturePaths[7] = {
        "assets/tetris/block_I.jpg", "assets/tetris/block_O.png", "assets/tetris/block_T.jpg",
        "assets/tetris/block_L.jpg", "assets/tetris/block_J.jpg", "assets/tetris/block_S.jpg", "assets/tetris/block_Z.jpg"
    };

    for (int i = 0; i < 7; i++) {
        blockTextures[i] = LoadTexture(texturePaths[i]);
        textureAvailable[i] = blockTextures[i].width > 0;
    }
}

void DefinePieces() {
    pieces[I] = {{{{0,0,0,0},{1,1,1,1},{0,0,0,0},{0,0,0,0}},{{0,0,1,0},{0,0,1,0},{0,0,1,0},{0,0,1,0}}}, 2, SKYBLUE, I};
    pieces[O] = {{{{0,1,1,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}}}, 1, YELLOW, O};
    pieces[T] = {{{{0,1,0,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}},{{0,1,0,0},{0,1,1,0},{0,1,0,0},{0,0,0,0}},{{0,0,0,0},{1,1,1,0},{0,1,0,0},{0,0,0,0}},{{0,1,0,0},{1,1,0,0},{0,1,0,0},{0,0,0,0}}}, 4, PURPLE, T};
    pieces[L] = {{{{0,0,1,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}},{{0,1,0,0},{0,1,0,0},{0,1,1,0},{0,0,0,0}},{{0,0,0,0},{1,1,1,0},{1,0,0,0},{0,0,0,0}},{{1,1,0,0},{0,1,0,0},{0,1,0,0},{0,0,0,0}}}, 4, ORANGE, L};
    pieces[J] = {{{{1,0,0,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}},{{0,1,1,0},{0,1,0,0},{0,1,0,0},{0,0,0,0}},{{0,0,0,0},{1,1,1,0},{0,0,1,0},{0,0,0,0}},{{0,1,0,0},{0,1,0,0},{1,1,0,0},{0,0,0,0}}}, 4, BLUE, J};
    pieces[S] = {{{{0,1,1,0},{1,1,0,0},{0,0,0,0},{0,0,0,0}},{{0,1,0,0},{0,1,1,0},{0,0,1,0},{0,0,0,0}}}, 2, GREEN, S};
    pieces[Z] = {{{{1,1,0,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}},{{0,0,1,0},{0,1,1,0},{0,1,0,0},{0,0,0,0}}}, 2, RED, Z};
}

void SpawnPiece() {
    currentPieceType = (PieceType)(rand() % 7);
    currentRotation = 0;
    blockPos = { GRID_WIDTH / 2 - 2, 0 };
}

bool CanMove(int dx, int dy, int newRotation = -1) {
    if (newRotation == -1) newRotation = currentRotation;
    const int (*shape)[4] = pieces[currentPieceType].rotations[newRotation];
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            if (shape[y][x]) {
                int nx = (int)blockPos.x + x + dx;
                int ny = (int)blockPos.y + y + dy;
                if (nx < 0 || nx >= GRID_WIDTH || ny >= GRID_HEIGHT || (ny >= 0 && grid[ny][nx]))
                    return false;
            }
        }
    }
    return true;
}

void FixPiece() {
    const int (*shape)[4] = pieces[currentPieceType].rotations[currentRotation];
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            if (shape[y][x]) {
                int gx = (int)blockPos.x + x;
                int gy = (int)blockPos.y + y;
                if (gx >= 0 && gx < GRID_WIDTH && gy >= 0 && gy < GRID_HEIGHT) {
                    grid[gy][gx] = (int)currentPieceType + 1;
                }
            }
        }
    }
}

void ClearFullLines() {
    int linesCleared = 0;
    for (int y = GRID_HEIGHT - 1; y >= 0; y--) {
        bool full = true;
        for (int x = 0; x < GRID_WIDTH; x++) {
            if (grid[y][x] == 0) {
                full = false;
                break;
            }
        }
        if (full) {
            linesCleared++;
            for (int row = y; row > 0; row--)
                for (int col = 0; col < GRID_WIDTH; col++)
                    grid[row][col] = grid[row - 1][col];
            for (int col = 0; col < GRID_WIDTH; col++)
                grid[0][col] = 0;
            y++;
        }
    }
    score += linesCleared * 100;
}

bool CheckGameOver() {
    for (int y = 0; y < GAME_OVER_LINE; y++)
        for (int x = 0; x < GRID_WIDTH; x++)
            if (grid[y][x]) return true;
    return false;
}

void ResetGame() {
    for (int y = 0; y < GRID_HEIGHT; y++)
        for (int x = 0; x < GRID_WIDTH; x++)
            grid[y][x] = 0;
    score = 0;
    isGameOver = false;
    SpawnPiece();
    fallTimer = 0.0f;
}

void InitMiniGame5() {
    LoadBlockTextures();
    DefinePieces();
    srand((unsigned int)time(nullptr));
    ResetGame();
}

void UpdateMiniGame5(GameState& gameState) {
    if (IsKeyPressed(KEY_BACKSPACE)) {
        UnloadMiniGame5();
        gameState = GameState::MENU;
        return;
    }

    if (isGameOver) {
        if (IsKeyPressed(KEY_SPACE)) {
            ResetGame();
        }
        return;
    }

    if (IsKeyPressed(KEY_A) && CanMove(-1, 0)) blockPos.x -= 1;
    if (IsKeyPressed(KEY_D) && CanMove(1, 0)) blockPos.x += 1;
    if (IsKeyPressed(KEY_W)) {
        int nextRotation = (currentRotation + 1) % pieces[currentPieceType].numRotations;
        if (CanMove(0, 0, nextRotation)) currentRotation = nextRotation;
    }

    float currentFallDelay = IsKeyDown(KEY_S) ? fastFallDelay : fallDelay;
    fallTimer += GetFrameTime();
    if (fallTimer >= currentFallDelay) {
        fallTimer = 0.0f;
        if (CanMove(0, 1)) {
            blockPos.y += 1;
        } else {
            FixPiece();
            ClearFullLines();
            if (CheckGameOver()) isGameOver = true;
            else SpawnPiece();
        }
    }
}

void DrawMiniGame5() {
    ClearBackground(RAYWHITE);
    DrawRectangleLinesEx({(float)OFFSET_X - 2, (float)OFFSET_Y - 2, (float)GRID_PIXEL_WIDTH + 4, (float)GRID_PIXEL_HEIGHT + 4}, 4, DARKGRAY);
    DrawLine(OFFSET_X, OFFSET_Y + GAME_OVER_LINE * CELL_SIZE, OFFSET_X + GRID_PIXEL_WIDTH, OFFSET_Y + GAME_OVER_LINE * CELL_SIZE, RED);

    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            int id = grid[y][x];
            if (id > 0) {
                int px = OFFSET_X + x * CELL_SIZE;
                int py = OFFSET_Y + y * CELL_SIZE;
                int index = id - 1;
                if (textureAvailable[index])
                    DrawTexture(blockTextures[index], px, py, WHITE);
                else {
                    DrawRectangle(px, py, CELL_SIZE, CELL_SIZE, pieces[index].color);
                    DrawRectangleLines(px, py, CELL_SIZE, CELL_SIZE, BLACK);
                }
            }
        }
    }

    if (!isGameOver) {
        const int (*shape)[4] = pieces[currentPieceType].rotations[currentRotation];
        for (int y = 0; y < 4; y++) {
            for (int x = 0; x < 4; x++) {
                if (shape[y][x]) {
                    int px = (int)(blockPos.x + x);
                    int py = (int)(blockPos.y + y);
                    if (py >= 0) {
                        int sx = OFFSET_X + px * CELL_SIZE;
                        int sy = OFFSET_Y + py * CELL_SIZE;
                        if (textureAvailable[currentPieceType])
                            DrawTexture(blockTextures[currentPieceType], sx, sy, WHITE);
                        else {
                            DrawRectangle(sx, sy, CELL_SIZE, CELL_SIZE, pieces[currentPieceType].color);
                            DrawRectangleLines(sx, sy, CELL_SIZE, CELL_SIZE, BLACK);
                        }
                    }
                }
            }
        }
    }

    DrawText(TextFormat("Score : %d", score), 10, 10, 24, DARKGRAY);
    if (isGameOver)
        DrawText("GAME OVER - ESPACE pour recommencer", 180, 280, 20, RED);
}

void UnloadMiniGame5() {
    for (int i = 0; i < 7; i++) {
        if (textureAvailable[i]) UnloadTexture(blockTextures[i]);
    }
}
