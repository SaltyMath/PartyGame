#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <cstdlib>
#include <curl/curl.h>
#include <windows.h>

#define CURRENT_VERSION "1.0.1"
#define VERSION_URL "https://raw.githubusercontent.com/SaltyMath/PartyGame/main/version.txt"
#define ZIP_URL "https://github.com/SaltyMath/PartyGame/releases/download/v1.0.1/game.zip"
#define GAME_ZIP "game.zip"
#define GAME_EXE "game.exe"

// Callback pour curl (texte)
size_t WriteToString(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// Télécharge un fichier texte (version.txt)
bool DownloadTextFile(const std::string& url, std::string& output) {
    CURL* curl = curl_easy_init();
    if (!curl) return false;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteToString);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &output);
    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    return res == CURLE_OK;
}

// Télécharge un fichier binaire (.zip)
bool DownloadFile(const std::string& url, const std::string& filename) {
    CURL* curl = curl_easy_init();
    if (!curl) return false;

    FILE* file = fopen(filename.c_str(), "wb");
    if (!file) return false;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);
    CURLcode res = curl_easy_perform(curl);
    fclose(file);
    curl_easy_cleanup(curl);
    return res == CURLE_OK;
}

// Dézip avec 7z.exe (doit être présent)
bool ExtractZip(const std::string& zipFile) {
    std::string cmd = "7z x \"" + zipFile + "\" -y";
    int result = system(cmd.c_str());
    return result == 0;
}

// Lancer le jeu
void LaunchGame() {
    ShellExecuteA(NULL, "open", GAME_EXE, NULL, NULL, SW_SHOWDEFAULT);
}

int main() {
    std::cout << "=== Launcher MathieuParty ===\n";

    std::string onlineVersion;
    if (!DownloadTextFile(VERSION_URL, onlineVersion)) {
        std::cerr << "Erreur : impossible de vérifier la version distante.\n";
        LaunchGame(); // On lance le jeu quand même
        return 1;
    }

    // Nettoyage de la chaîne (trim)
    onlineVersion.erase(onlineVersion.find_last_not_of(" \n\r\t") + 1);

    if (onlineVersion != CURRENT_VERSION) {
        std::cout << "Nouvelle version disponible : " << onlineVersion << "\n";
        std::cout << "Téléchargement de la mise à jour...\n";

        if (!DownloadFile(ZIP_URL, GAME_ZIP)) {
            std::cerr << "Erreur : échec du téléchargement.\n";
            return 1;
        }

        std::cout << "Décompression...\n";
        if (!ExtractZip(GAME_ZIP)) {
            std::cerr << "Erreur : impossible de décompresser.\n";
            return 1;
        }

        std::cout << "Mise à jour terminée.\n";
    } else {
        std::cout << "Aucune mise à jour disponible. Version actuelle : " << CURRENT_VERSION << "\n";
    }

    // Tenter de cacher game.exe (optionnel)
    if (SetFileAttributesA(GAME_EXE, FILE_ATTRIBUTE_HIDDEN)) {
        std::cout << GAME_EXE << " a été caché.\n";
    } else {
        std::cerr << "Échec du masquage de " << GAME_EXE << "\n";
    }

    std::cout << "Lancement du jeu...\n";
    LaunchGame();
    return 0;
}
