#include "TitleScreen.hpp"
#include "raylib.h"

static Texture2D titleImage;

void InitTitleScreen()  {
    titleImage = LoadTexture("assets/Logo.png");
}

void UnloadTitleScreen() {
    UnloadTexture(titleImage);
}

void UpdateTitleScreen(GameState& state) {
    if (IsKeyPressed(KEY_ENTER)) {
        state = GameState::MENU;
    }
}

void DrawTitleScreen() {
    ClearBackground(BLACK);

    Rectangle source = { 0.0f, 0.0f, (float)titleImage.width, (float)titleImage.height };
    Vector2 centerScreen = { GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f };
    Vector2 origin = { titleImage.width / 2.0f, titleImage.height / 2.0f };

    DrawTexturePro(titleImage, source,
                    (Rectangle){ centerScreen.x, centerScreen.y, (float)titleImage.width, (float)titleImage.height },
                    origin, 0.0f, WHITE);

    const char* msg = "Appuyez sur [ENTREE] pour continuer";
    int textWidth = MeasureText(msg, 20);
    DrawText(msg, GetScreenWidth() / 2 - textWidth/2, GetScreenHeight() - 40, 20, DARKGRAY);
}
