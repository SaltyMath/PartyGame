#include "MiniGame1.hpp"
#include "raylib.h"
#include <string>

static Texture2D terrain;
static Vector2 ballePosition = { 0 };
static bool tirEnCours = false;
static bool tirEffectue = false;
static float balleVitesse = 5.0f;
static std::string messageAction = "";
static std::string messageResultat = "";
static char choixJ1 = 0;
static char choixJ2 = 0;
static Texture2D spriteTireur;
static Texture2D spriteGardien;
static int frameTireur = 0;
static int frameGardien = 0;
static float animTimerTireur = 0.0f;
static float animTimerGardien = 0.0f;
const float FRAME_TIME = 0.15f;
static Texture2D spriteBuisson;

// Initialisation
void InitMiniGame1() {
    terrain = LoadTexture("assets/foot/MiniJeu1.png");
    spriteTireur = LoadTexture("assets/foot/Joueur_Foot.png");
    spriteGardien = LoadTexture("assets/foot/Gardien_Foot.png");
    spriteBuisson = LoadTexture("assets/foot/buisson.png");
    frameTireur = 0;
    frameGardien = 0;
    animTimerTireur = 0.0f;
    animTimerGardien = 0.0f;

    if (terrain.id <= 0) {
        TraceLog(LOG_ERROR, "Erreur de chargement de MiniJeu1.png");
    }

    ballePosition = { 400, 430 };
    tirEnCours = false;
    tirEffectue = false;
    messageAction = "";
    messageResultat = "";
    choixJ1 = 0;
    choixJ2 = 0;
}

void UpdateMiniGame1(GameState& gameState) {
    if (IsKeyPressed(KEY_BACKSPACE)) {
        gameState = GameState::MENU;
        return;
    }

    // Si le tir n'a pas encore été effectué
    if (!tirEffectue) {
        // J1 : touche A/S/D/F
        if (IsKeyPressed(KEY_A)) choixJ1 = 'A';
        if (IsKeyPressed(KEY_S)) choixJ1 = 'S';
        if (IsKeyPressed(KEY_D)) choixJ1 = 'D';
        if (IsKeyPressed(KEY_F)) choixJ1 = 'F';

        // J2 : touche Q/W/E/R
        if (IsKeyPressed(KEY_Q)) choixJ2 = 'Q';
        if (IsKeyPressed(KEY_W)) choixJ2 = 'W';
        if (IsKeyPressed(KEY_E)) choixJ2 = 'E';
        if (IsKeyPressed(KEY_R)) choixJ2 = 'R';

        // Le joueur ne tire qu'après le choix du J2
        if (choixJ1 != '\0' && choixJ2 != '\0') {
            tirEffectue = true;
            tirEnCours = true;
            messageAction = "Le joueur s'élance... et tire !";

            bool arret = false;
            if ((choixJ1 == 'A' && choixJ2 == 'Q') ||
                (choixJ1 == 'S' && choixJ2 == 'W') ||
                (choixJ1 == 'D' && choixJ2 == 'E') ||
                (choixJ1 == 'F' && choixJ2 == 'R')) {
                arret = true;
            }

            if (arret) {
                messageResultat = "OOOH L'ARRÊÊÊÊT !!";
            } else {
                messageResultat = "BUUUUUUT !";
            }
        }
    }

    // Déplacement de la balle vers la droite
    if (tirEnCours) {
        ballePosition.x += balleVitesse;

        // Si arrêt la balle s'arrête avant le but
        float limite = (messageResultat == "OOOH L'ARRÊÊÊÊT !!") ? 700.0f : 800.0f;

        if (ballePosition.x > limite) {
            tirEnCours = false;
        }

    }

    // Reset manuel avec espace après le tir
    if (tirEffectue && !tirEnCours && IsKeyPressed(KEY_SPACE)) {
        ballePosition = { 400, 430 };
        tirEffectue = false;
        tirEnCours = false;
        choixJ1 = '\0';
        choixJ2 = '\0';
        messageAction = "";
        messageResultat = "";
    }

    // Animation tireur
    animTimerTireur += GetFrameTime();
    if (animTimerTireur >= FRAME_TIME) {
        animTimerTireur = 0.0f;
        frameTireur = (frameTireur + 1) % 6; // 6 frames
    }

    // Animation gardien
    animTimerGardien += GetFrameTime();
    if (animTimerGardien >= FRAME_TIME) {
        animTimerGardien = 0.0f;
        frameGardien = (frameGardien + 1) % 4; // 4 frames
    }

}


void DrawMiniGame1() {
    ClearBackground(RAYWHITE);

    Rectangle src = { 0, 0, (float)terrain.width, (float)terrain.height };
    Rectangle dest = { 0, 0, 800, 600 }; // taille de la fenêtre
    Vector2 origin = { 0, 0 };
    DrawTexturePro(terrain, src, dest, origin, 0.0f, WHITE);
    

    DrawCircleV(ballePosition, 10, WHITE); // Dessine la balle
    DrawTexture(spriteBuisson, 770, 408, WHITE); // buisson positionné vers le fond
    // Messages
    if (!messageAction.empty())
        DrawText(messageAction.c_str(), 200, 500, 20, BLACK);
    if (!messageResultat.empty())
        DrawText(messageResultat.c_str(), 250, 530, 30, RED);

    Rectangle frameTireur = { 0, 0, 48, 48 }; 
    Vector2 posTireur = { 370, 392 };        
    DrawTextureRec(spriteTireur, frameTireur, posTireur, WHITE);
    
    Rectangle frameGardien = { 0, 0, 48, 48 };
    Vector2 posGardien = { 700, 392 };
    DrawTextureRec(spriteGardien, frameGardien, posGardien, WHITE);

}

void UnloadMiniGame1() {
    UnloadTexture(terrain);
    UnloadTexture(spriteTireur);
    UnloadTexture(spriteGardien);
    UnloadTexture(spriteBuisson);
}