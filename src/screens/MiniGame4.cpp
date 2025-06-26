#include "MiniGame4.hpp"
#include "raylib.h"
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <queue>
#include <string>

static const int SCREEN_WIDTH = 800;
static const int SCREEN_HEIGHT = 600;
static const int TILE_SIZE = 40;
static const int MAP_COLS = SCREEN_WIDTH / TILE_SIZE;
static const int MAP_ROWS = SCREEN_HEIGHT / TILE_SIZE;

enum Team { PLAYER = 0, ENEMY = 1 };

enum MiniGame4State {
    MAP_SELECTION,
    GAME_RUNNING,
    GAME_OVER
};

static MiniGame4State game4State = MAP_SELECTION;

struct UnitType {
    std::string name;
    int moveRange;
    int attackRange;
    int maxHp;
    int attack;
};

UnitType swordsman = {"swordsman", 4, 1, 10, 3};
UnitType soldier = {"Soldat", 2, 1, 7, 2};
UnitType knight = {"Chevalier", 3, 1, 15, 4};
UnitType paladin = {"Paladin", 1, 1, 30, 6};

std::vector<UnitType> availableUnitTypes = { swordsman, soldier, knight, paladin };

struct Unit {
    int x, y;
    int moveRange;
    int attackRange;
    int hp;
    int maxHp;
    int attack;
    Team team;
    Color color;
    bool hasActed;
    Texture2D sprite;
    bool hasTexture;
};

static std::vector<Unit> playerUnits;
static std::vector<Unit> enemyUnits;
static bool mapObstacles[MAP_COLS][MAP_ROWS];
static bool moveZone[MAP_COLS][MAP_ROWS];
static int selectedUnitIndex = -1;
static bool unitSelected = false;
static bool playerTurn = true;
static int cursorX = -1, cursorY = -1;
static std::string currentMap;
static Texture2D terrain;
static Texture2D mapPreviews[5];
static Rectangle mapRects[5];
static int selectedPreview = -1;
static bool soloMode = true;

static Texture2D playerSprites[4];
static Texture2D enemySprites[4];

float obstacleAlpha = 0.0f; // Opacité obstacles

void DrawObstacles() {
    for (int x = 0; x < MAP_COLS; x++) {
        for (int y = 0; y < MAP_ROWS; y++) {
            if (mapObstacles[x][y]) {
                DrawRectangle(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE, Fade(DARKBROWN, obstacleAlpha));
            }
        }
    }
}

void DrawUnit(const Unit& unit) {
    if (unit.hp <= 0) return;

    Rectangle dest = { (float)(unit.x * TILE_SIZE + 4), (float)(unit.y * TILE_SIZE + 4), TILE_SIZE - 8, TILE_SIZE - 8 };
    if (unit.hasTexture) {
        DrawTexturePro(unit.sprite, { 0, 0, (float)unit.sprite.width, (float)unit.sprite.height }, dest, { 0, 0 }, 0, WHITE);
    } else {
        DrawRectangleRec(dest, unit.color);
    }

    // Draw HP bar
    float hpRatio = (float)unit.hp / unit.maxHp;
    DrawRectangle(dest.x, dest.y - 6, dest.width, 4, DARKGRAY);
    DrawRectangle(dest.x, dest.y - 6, dest.width * hpRatio, 4, GREEN);
}

void LoadMap(const std::string& mapName) {
    currentMap = mapName;
    for (int x = 0; x < MAP_COLS; x++)
        for (int y = 0; y < MAP_ROWS; y++)
            mapObstacles[x][y] = false;

    if (mapName == "Map001") {
        int coords[][2] = {{0,0}, {1,0}, {2,0}, {3,0}, {4,0}, {5,0}, {19,0}, {0,1}, {1,1}, {2,1}, {3,1}, {4,1}, {5,1}, {19,1}, {5,2}, {6,2}, {5,3}, {6,3}, {5,4}, {6,4}, {4,6}, {5,6}, {6,6}, {3,7}, {4,7}, {5,7}, {6,7}, {3,8}, {4,8}, {5,8}, {2,9}, {0,10}, {1,10}, {2,10}, {0,11}, {1,11}, {2,11}, {0,12}, {1,12}, {2,12}, {3,12}, {0,13}, {1,13}, {2,13}, {3,13}, {4,13}, {5,13}, {6,13}, {7,13}, {0,14}, {1,14}, {2,14}, {3,14}, {4,14}, {5,14}, {6,14}, {7,14}, {8,14}, {9,14}, {10,14}, {11,14}, {12,14}, {13,14}, {14,14}, {15,14}, {16,14}, {17,14}, {18,14}, {19,14}, {15,11}, {15,12}, {19,2}, {19,3}, {19,4}, {19,5}, {19,6}, {19,7}, {19,8}, {19,9}, {19,10}, {19,11}, {19,12}, {19,13}, {19,14}};
        for (auto& c : coords) mapObstacles[c[0]][c[1]] = true;
    } else if (mapName == "Map002") {
        int coords[][2] = {{2,7}, {2,8}, {2,9}, {3,7}, {3,8}, {3,9}, {0,13}, {1,13}, {0,14}, {1,14}, {15,13}, {16,13}, {17,13}, {18,13}, {19,13}, {15,14}, {16,14}, {17,14}, {18,14}, {19,14}, {11,2}, {11,3}, {11,4}, {12,2}, {12,3}, {12,4}, {12,5}, {13,2}, {13,3}, {13,4}, {13,5}, {13,6}, {14,2}, {14,3}, {14,4}, {14,5}, {14,6}, {14,7}, {15,2}, {15,3}, {15,4}, {15,5}, {15,6}, {15,7}, {15,8}, {16,2}, {16,3}, {16,4}, {16,5}, {16,6}, {16,7}, {16,8}, {16,9}, {17,2}, {17,3}, {17,4}, {17,5}, {17,6}, {17,7}, {17,8}, {17,9}};
        for (auto& c : coords) mapObstacles[c[0]][c[1]] = true;
    } else if (mapName == "Map003") {
        int coords[][2] = {{5,4}, {6,4}, {7,4}, {8,4}, {9,4}, {5,5}, {6,5}, {7,5}, {8,5}, {9,5}, {10,5}, {1,6}, {5,6}, {6,6}, {7,6}, {8,6}, {9,6}, {10,6}, {6,7}, {7,7}, {8,7}, {9,7}, {14,10}, {15,10}};
        for (auto& c : coords) mapObstacles[c[0]][c[1]] = true;
    } else if (mapName == "Map004") {
        int coords[][2] = {{0,7}, {0,8}, {0,9}, {1,7}, {1,8}, {1,9}, {1,10}, {1,11}, {2,7}, {2,8}, {2,9}, {3,7}, {3,8}, {3,9}, {4,7}, {4,8}, {4,9}, {4,10}, {4,11}, {4,12}, {5,7}, {5,8}, {5,9}, {5,10}, {5,11}, {5,12}, {10,12}, {10,13}, {11,12}, {11,13}, {12,12}, {12,13}, {13,12}, {13,13}};
        for (auto& c : coords) mapObstacles[c[0]][c[1]] = true;
    } else if (mapName == "Map005") {
        int coords[][2] = {{6,0}, {7,0}, {8,0}, {9,0}, {10,0}, {6,1}, {7,1}, {8,1}, {9,1}, {10,1}, {19,0}, {18,0}, {17,0}, {19,1}, {18,1}, {17,1}, {16,0}, {19,2}, {18,2}, {19,3}, {6,4}, {6,5}, {6,6}, {6,7}, {6,8}, {6,9}, {6,10}, {7,4}, {7,5}, {7,6}, {7,7}, {7,8}, {7,9}, {7,10}, {8,4}, {8,5}, {8,6}, {8,7}, {8,8}, {8,9}, {8,10}, {9,4}, {9,5}, {9,6}, {9,7}, {9,8}, {9,9}, {9,10}, {10,4}, {10,5}, {10,6}, {10,7}, {10,8}, {10,9}, {10,10}, {11,4}, {11,5}, {11,6}, {11,7}, {11,8}, {11,9}, {11,10}, {12,4}, {12,5}, {12,6}, {12,7}, {12,8}, {12,9}, {12,10}};
        for (auto& c : coords) mapObstacles[c[0]][c[1]] = true;
    }

    terrain = LoadTexture(("assets/FE/" + mapName + ".png").c_str());
}

void InitMiniGame4() {
    for (int i = 0; i < 5; i++) {
        std::string name = "assets/FE/Map00" + std::to_string(i + 1) + ".png";
        mapPreviews[i] = LoadTexture(name.c_str());
        mapRects[i] = { 40 + i * 150.0f, 100, 120, 90 };
    }
    game4State = MAP_SELECTION;
}

void StartGameFromSelection() {
    playerUnits.clear();
    enemyUnits.clear();
    srand(time(nullptr));  // Initialisation aléatoire

    // Chargement des sprites pour les 4 classes (joueur et joueur 2)
    const char* classNames[4] = {"swordsman", "Soldat", "Chevalier", "Paladin"};
    for (int i = 0; i < 4; i++) {
    std::string playerPath = "assets/FE/player_" + std::string(classNames[i]) + ".png";
    std::string enemyPath  = "assets/FE/enemy_"  + std::string(classNames[i]) + ".png";

    if (FileExists(playerPath.c_str())) {
        playerSprites[i] = LoadTexture(playerPath.c_str());
    } else {
        TraceLog(LOG_WARNING, "PLAYER sprite missing: %s", playerPath.c_str());
        playerSprites[i] = Texture2D{};
    }

    if (FileExists(enemyPath.c_str())) {
        enemySprites[i] = LoadTexture(enemyPath.c_str());
    } else {
        TraceLog(LOG_WARNING, "ENEMY sprite missing: %s", enemyPath.c_str());
        enemySprites[i] = Texture2D{};
    }
}

    // Tableau de positions sûres par carte (modifiable facilement)
std::vector<Vector2> playerStarts;
std::vector<Vector2> enemyStarts;

switch (selectedPreview) {
    case 0: // Map001
        playerStarts = { {2,5}, {8,5}, {14,3}, {10,1} };
        enemyStarts  = { {10,12}, {7,12}, {11,11}, {18,12} };
        break;
    case 1: // Map002
        playerStarts = { {2,3}, {3,3}, {2,4}, {3,4} };
        enemyStarts  = { {13,12}, {14,12}, {13,11}, {14,11} };
        break;
    case 2: // Map003
        playerStarts = { {1,10}, {2,9}, {3,9}, {4,10} };
        enemyStarts  = { {15,2}, {16,3}, {17,3}, {18,2} };
        break;
    case 3: // Map004
        playerStarts = { {2,13}, {3,14}, {18,11}, {17,10} };
        enemyStarts  = { {16,1}, {17,2}, {2,2}, {1,1} };
        break;
    case 4: // Map005
        playerStarts = { {9,3}, {2,1}, {15,3}, {4,1} };
        enemyStarts  = { {15,10}, {3,10}, {17,10}, {9,11} };
        break;
}


    for (int i = 0; i < 4; ++i) {
    int idxP = rand() % availableUnitTypes.size();
    UnitType type = availableUnitTypes[idxP];
    Vector2 posP = playerStarts[i];
    playerUnits.push_back({
        (int)posP.x, (int)posP.y,
        type.moveRange, type.attackRange,
        type.maxHp, type.maxHp, type.attack,
        PLAYER, BLUE, false,
        playerSprites[idxP], true
    });

    int idxE = rand() % availableUnitTypes.size();
    UnitType enemyType = availableUnitTypes[idxE];
    Vector2 posE = enemyStarts[i];
    enemyUnits.push_back({
        (int)posE.x, (int)posE.y,
        enemyType.moveRange, enemyType.attackRange,
        enemyType.maxHp, enemyType.maxHp, enemyType.attack,
        ENEMY, RED, false,
        enemySprites[idxE], true
    });
}

    playerTurn = true;
    unitSelected = false;
    selectedUnitIndex = -1;
    cursorX = -1;
    cursorY = -1;

    LoadMap("Map00" + std::to_string(selectedPreview + 1));
    game4State = GAME_RUNNING;
}

bool IsTileOccupied(int x, int y) {
    for (const auto& unit : playerUnits)
        if (unit.hp > 0 && unit.x == x && unit.y == y)
            return true;
    for (const auto& unit : enemyUnits)
        if (unit.hp > 0 && unit.x == x && unit.y == y)
            return true;
    return false;
}

void ComputeMoveZone(int startX, int startY, int range) {
    for (int x = 0; x < MAP_COLS; x++)
        for (int y = 0; y < MAP_ROWS; y++)
            moveZone[x][y] = false;

    struct Node { int x, y, cost; };
    std::queue<Node> q;
    q.push({ startX, startY, 0 });
    moveZone[startX][startY] = true;

    while (!q.empty()) {
        Node node = q.front(); q.pop();
        if (node.cost >= range) continue;

        const int dx[4] = { 0, 0, -1, 1 };
        const int dy[4] = { -1, 1, 0, 0 };

        for (int i = 0; i < 4; i++) {
            int nx = node.x + dx[i];
            int ny = node.y + dy[i];
            if (nx >= 0 && ny >= 0 && nx < MAP_COLS && ny < MAP_ROWS && !mapObstacles[nx][ny] && !moveZone[nx][ny]) {
                moveZone[nx][ny] = true;
                q.push({ nx, ny, node.cost + 1 });
            }
        }
    }
}

bool IsInRange(int x1, int y1, int x2, int y2, int range) {
    return abs(x1 - x2) + abs(y1 - y2) <= range;
}

Unit* GetAllyAt(int x, int y, Team team) {
    std::vector<Unit>& allies = (team == PLAYER) ? playerUnits : enemyUnits;
    for (auto& u : allies)
        if (u.hp > 0 && u.x == x && u.y == y)
            return &u;
    return nullptr;
}

Unit* GetEnemyAt(int x, int y, Team team) {
    std::vector<Unit>& enemies = (team == PLAYER) ? enemyUnits : playerUnits;
    for (auto& u : enemies)
        if (u.hp > 0 && u.x == x && u.y == y)
            return &u;
    return nullptr;
}

void Attack(Unit& attacker, Unit& defender) {
    defender.hp -= attacker.attack;
    if (defender.hp < 0) defender.hp = 0;
    attacker.hasActed = true;
}

void UpdateMiniGame4(GameState& gameState) {
    if (game4State == MAP_SELECTION) {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 mouse = GetMousePosition();
            for (int i = 0; i < 5; i++) {
                if (CheckCollisionPointRec(mouse, mapRects[i])) {
                    selectedPreview = i;
                }
            }
        }
        if (selectedPreview != -1) {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) { soloMode = true; StartGameFromSelection(); }
        }
        return;
    }

    if (game4State == GAME_OVER) {
        if (IsKeyPressed(KEY_ENTER)) {
            UnloadTexture(terrain);
            InitMiniGame4();
            gameState = GameState::MENU;
        }
        return;
    }

    if (IsKeyPressed(KEY_BACKSPACE)) {
        UnloadTexture(terrain);
        InitMiniGame4();
        gameState = GameState::MENU;
        return;
    }

    // Fin de partie si un camp est éliminé
    bool anyPlayerAlive = false;
    for (const auto& u : playerUnits) if (u.hp > 0) anyPlayerAlive = true;
    bool anyEnemyAlive = false;
    for (const auto& u : enemyUnits) if (u.hp > 0) anyEnemyAlive = true;

    if (!anyPlayerAlive || !anyEnemyAlive) {
        game4State = GAME_OVER;
        return;
    }

    std::vector<Unit>& currentUnits = playerTurn ? playerUnits : enemyUnits;
    Team currentTeam = playerTurn ? PLAYER : ENEMY;

    if (!unitSelected && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 mouse = GetMousePosition();
        int tileX = mouse.x / TILE_SIZE;
        int tileY = mouse.y / TILE_SIZE;

        for (size_t i = 0; i < currentUnits.size(); i++) {
            if (currentUnits[i].x == tileX && currentUnits[i].y == tileY &&
                !currentUnits[i].hasActed && currentUnits[i].hp > 0) {
                selectedUnitIndex = i;
                unitSelected = true;
                cursorX = tileX;
                cursorY = tileY;
                ComputeMoveZone(tileX, tileY, currentUnits[i].moveRange);
                break;
            }
        }
    }

    if (unitSelected && selectedUnitIndex != -1) {
        if (IsKeyPressed(KEY_W) && cursorY > 0) cursorY--;
        if (IsKeyPressed(KEY_S) && cursorY < MAP_ROWS - 1) cursorY++;
        if (IsKeyPressed(KEY_A) && cursorX > 0) cursorX--;
        if (IsKeyPressed(KEY_D) && cursorX < MAP_COLS - 1) cursorX++;

        if (IsKeyPressed(KEY_SPACE)) {
            Unit& unit = currentUnits[selectedUnitIndex];
            if (cursorX >= 0 && cursorY >= 0 && cursorX < MAP_COLS && cursorY < MAP_ROWS &&
                moveZone[cursorX][cursorY] && !mapObstacles[cursorX][cursorY]) {

                Unit* enemy = GetEnemyAt(cursorX, cursorY, currentTeam);
                Unit* ally = GetAllyAt(cursorX, cursorY, currentTeam);

                if (enemy) {
                    int dx = abs(enemy->x - unit.x);
                    int dy = abs(enemy->y - unit.y);
                    if ((dx + dy) <= unit.attackRange && unit.attack > 0) {
                        enemy->hp -= unit.attack;
                        if (enemy->hp < 0) enemy->hp = 0;
                        unit.hasActed = true;
                        unitSelected = false;
                        selectedUnitIndex = -1;
                    }
                } else if (!ally) {
                    unit.x = cursorX;
                    unit.y = cursorY;
                    unit.hasActed = true;
                    unitSelected = false;
                    selectedUnitIndex = -1;
                }
            }
        }
    }

    bool allActed = true;
    for (const auto& unit : currentUnits) {
        if (!unit.hasActed && unit.hp > 0) {
            allActed = false;
            break;
        }
    }
    if (allActed) {
        for (auto& unit : currentUnits) unit.hasActed = false;
        playerTurn = !playerTurn;
        unitSelected = false;
        selectedUnitIndex = -1;
    }
}

void DrawMiniGame4() {
    if (game4State == MAP_SELECTION) {
        ClearBackground(BLACK);
        for (int i = 0; i < 5; i++) {
            DrawTexturePro(mapPreviews[i], { 0,0,(float)mapPreviews[i].width, (float)mapPreviews[i].height }, mapRects[i], { 0,0 }, 0, WHITE);
            if (i == selectedPreview) DrawRectangleLinesEx(mapRects[i], 2, YELLOW);
        }
        return;
    }

    if (game4State == GAME_OVER) {
        ClearBackground(BLACK);
        const char* winner = "";
        bool anyPlayerAlive = false;
        for (const auto& u : playerUnits) if (u.hp > 0) anyPlayerAlive = true;
        winner = anyPlayerAlive ? "Victoire Joueur 1 !" : "Victoire Joueur 2 !";
        DrawText(winner, 260, 250, 30, RAYWHITE);
        DrawText("Appuyez sur Entrée pour revenir au menu", 200, 300, 20, GRAY);
        return;
    }

    ClearBackground(DARKGREEN);
    DrawTextureEx(terrain, {0, 0}, 0.0f, 1.0f, WHITE);
    DrawObstacles();

    for (int x = 0; x < MAP_COLS; x++) {
        for (int y = 0; y < MAP_ROWS; y++) {
            DrawRectangleLines(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE, DARKGRAY);
        }
    }

    if (unitSelected && selectedUnitIndex != -1) {
        for (int x = 0; x < MAP_COLS; x++) {
            for (int y = 0; y < MAP_ROWS; y++) {
                if (moveZone[x][y]) {
                    DrawRectangle(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE, Fade(YELLOW, 0.3f));
                }
            }
        }
    }

    for (const auto& unit : playerUnits) DrawUnit(unit);
    for (const auto& unit : enemyUnits) DrawUnit(unit);

    if (unitSelected && selectedUnitIndex != -1) {
        DrawRectangle(cursorX * TILE_SIZE + 4, cursorY * TILE_SIZE + 4, TILE_SIZE - 8, TILE_SIZE - 8, Fade(GREEN, 0.5f));
    }
}

void UnloadMiniGame4() {
    UnloadTexture(terrain);
    for (int i = 0; i < 5; i++) UnloadTexture(mapPreviews[i]);

    for (int i = 0; i < 4; i++) {
        if (playerSprites[i].id != 0) UnloadTexture(playerSprites[i]);
        if (enemySprites[i].id != 0) UnloadTexture(enemySprites[i]);
    }
}