#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <cstdlib>
#include <windows.h>

#define CURRENT_VERSION "1.0.0"
#define VERSION_URL "https://raw.githubusercontent.com/SaltyMath/PartyGame/main/version.txt"
#define ZIP_URL "https://github.com/SaltyMath/PartyGame/releases/latest/download/game.zip"
#define GAME_ZIP "game.zip"
#define GAME_EXE "game.exe"

// Télécharge un fichier texte (version.txt)
bool DownloadTextFile(const std::string& url, std::string& output) {
    std::string command = "curl.exe -L \"" + url + "\" -o version_temp.txt";
    std::cout << "Téléchargement de version.txt avec : " << command << "\n";
    int result = system(command.c_str());
    if (result != 0) {
        std::cerr << "Erreur : curl a échoué (code " << result << ").\n";
        return false;
    }

    std::ifstream file("version_temp.txt");
    if (!file) {
        std::cerr << "Erreur : impossible d’ouvrir version_temp.txt.\n";
        return false;
    }

    std::getline(file, output);
    file.close();
    std::remove("version_temp.txt");
    return true;
}

// Télécharge game.zip avec curl.exe
bool DownloadFileWithCurl(const std::string& url, const std::string& filename) {
    std::string command = "curl.exe -L \"" + url + "\" -o \"" + filename + "\"";
    std::cout << "Téléchargement de " << filename << " avec : " << command << "\n";
    int result = system(command.c_str());
    if (result != 0) {
        std::cerr << "Erreur : curl a échoué (code " << result << ").\n";
        return false;
    }

    std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
    if (!in || in.tellg() == 0) {
        std::cerr << "Erreur : le fichier " << filename << " est vide.\n";
        std::remove(filename.c_str());
        return false;
    }

    std::cout << "Fichier téléchargé avec succès : " << filename << " (" << in.tellg() << " octets)\n";
    return true;
}

// Dézip avec 7z.exe (doit être présent dans le dossier)
bool ExtractZip(const std::string& zipFile) {
    std::string cmd = "7z x \"" + zipFile + "\" -y";
    int result = system(cmd.c_str());
    if (result != 0) {
        std::cerr << "Erreur : décompression avec 7z échouée (code " << result << ").\n";
    }
    return result == 0;
}

// Lancer le jeu
void LaunchGame() {
    std::cout << "Lancement du jeu...\n";
    ShellExecuteA(NULL, "open", GAME_EXE, NULL, NULL, SW_SHOWDEFAULT);
}

int main() {
    std::cout << "=== Launcher MathieuParty ===\n";

    std::string onlineVersion;
    if (!DownloadTextFile(VERSION_URL, onlineVersion)) {
        std::cerr << "Impossible de récupérer la version distante. Lancement du jeu quand même...\n";
        LaunchGame();
        return 1;
    }

    // Nettoyage de la chaîne (trim)
    onlineVersion.erase(onlineVersion.find_last_not_of(" \n\r\t") + 1);

    if (onlineVersion != CURRENT_VERSION) {
        std::cout << "Nouvelle version disponible : " << onlineVersion << "\n";
        std::cout << "Téléchargement de la mise à jour...\n";

        if (!DownloadFileWithCurl(ZIP_URL, GAME_ZIP)) {
            std::cerr << "Échec du téléchargement de la mise à jour.\n";
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

    // Optionnel : cacher le .exe du jeu
    if (SetFileAttributesA(GAME_EXE, FILE_ATTRIBUTE_HIDDEN)) {
        std::cout << GAME_EXE << " a été caché.\n";
    } else {
        std::cerr << "Impossible de cacher " << GAME_EXE << ".\n";
    }

    LaunchGame();
    return 0;
}
