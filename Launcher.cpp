#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <windows.h>
#include <shlwapi.h>  // Pour PathRemoveFileSpecA

#define CURRENT_VERSION "1.0.0"
const std::string VERSION_URL = "https://raw.githubusercontent.com/SaltyMath/PartyGame/main/version.txt";
const std::string ZIP_URL = "https://github.com/SaltyMath/PartyGame/releases/latest/download/game.zip";
const std::string GAME_ZIP = "game.zip";
const std::string GAME_EXE = "game.exe";

// Obtenir le dossier de l'exécutable
std::string GetExecutableDir() {
    char path[MAX_PATH];
    GetModuleFileNameA(NULL, path, MAX_PATH);
    PathRemoveFileSpecA(path);
    return std::string(path);
}

// Télécharger un fichier texte (version.txt) dans %TEMP%
bool DownloadTextFile(const std::string& url, std::string& output) {
    char tempPath[MAX_PATH];
    GetTempPathA(MAX_PATH, tempPath);
    std::string outFile = std::string(tempPath) + "version_temp.txt";
    std::string command = "curl.exe -L \"" + url + "\" -o \"" + outFile + "\"";
    std::cout << "Téléchargement : " << command << "\n";

    int result = system(command.c_str());
    if (result != 0) {
        std::cerr << "Erreur : curl a échoué (code " << result << ").\n";
        return false;
    }

    std::ifstream file(outFile);
    if (!file) {
        std::cerr << "Erreur : ouverture de version_temp.txt impossible.\n";
        return false;
    }

    std::getline(file, output);
    file.close();
    std::remove(outFile.c_str());

    // Nettoyage de la chaîne
    output.erase(output.find_last_not_of(" \n\r\t") + 1);
    return true;
}

// Télécharger un fichier binaire (game.zip) dans le dossier d'installation
bool DownloadFileWithCurl(const std::string& url, const std::string& filename, const std::string& exeDir) {
    std::string fullPath = exeDir + "\\" + filename;
    std::string command = "curl.exe -L \"" + url + "\" -o \"" + fullPath + "\"";
    std::cout << "Téléchargement de : " << fullPath << "\n";

    int result = system(command.c_str());
    if (result != 0) {
        std::cerr << "Erreur : curl a échoué (code " << result << ").\n";
        return false;
    }

    std::ifstream in(fullPath, std::ifstream::ate | std::ifstream::binary);
    if (!in || in.tellg() == 0) {
        std::cerr << "Erreur : fichier vide.\n";
        std::remove(fullPath.c_str());
        return false;
    }

    std::cout << "Téléchargement réussi (" << in.tellg() << " octets)\n";
    return true;
}

// Décompresser le zip avec 7z.exe dans le dossier d'installation
bool ExtractZip(const std::string& zipFile, const std::string& exeDir) {
    std::string fullZipPath = exeDir + "\\" + zipFile;
    std::string command = "cd /d \"" + exeDir + "\" && 7z.exe x \"" + fullZipPath + "\" -y";
    std::cout << "Commande de décompression : " << command << "\n";

    int result = system(command.c_str());
    if (result != 0) {
        std::cerr << "Erreur : décompression échouée (code " << result << ").\n";
    }
    return result == 0;
}

// Lancer le jeu
void LaunchGame(const std::string& exeDir) {
    std::string gamePath = exeDir + "\\" + GAME_EXE;
    ShellExecuteA(NULL, "open", gamePath.c_str(), NULL, exeDir.c_str(), SW_SHOWDEFAULT);
}

int main() {
    std::cout << "=== Launcher MathieuParty ===\n";

    std::string exeDir = GetExecutableDir();
    std::cout << "Répertoire d'installation détecté : " << exeDir << "\n";

    std::string onlineVersion;
    if (!DownloadTextFile(VERSION_URL, onlineVersion)) {
        std::cerr << "Impossible de récupérer la version en ligne.\n";
        std::cerr << "Lancement du jeu actuel...\n";
        LaunchGame(exeDir);
        return 1;
    }

    std::cout << "Version en ligne : " << onlineVersion << " | Locale : " << CURRENT_VERSION << "\n";

    if (onlineVersion != CURRENT_VERSION) {
        std::cout << "Mise à jour disponible. Téléchargement...\n";

        if (!DownloadFileWithCurl(ZIP_URL, GAME_ZIP, exeDir)) {
            std::cerr << "Erreur de téléchargement. Lancement annulé.\n";
            return 1;
        }

        std::cout << "Décompression...\n";
        if (!ExtractZip(GAME_ZIP, exeDir)) {
            std::cerr << "Échec de décompression. Lancement annulé.\n";
            return 1;
        }

        std::remove((exeDir + "\\" + GAME_ZIP).c_str());
        std::cout << "Mise à jour terminée !\n";
    } else {
        std::cout << "Aucune mise à jour. Version actuelle à jour.\n";
    }

    // Cacher le jeu
    std::string gamePath = exeDir + "\\" + GAME_EXE;
    if (SetFileAttributesA(gamePath.c_str(), FILE_ATTRIBUTE_HIDDEN)) {
        std::cout << GAME_EXE << " a été caché.\n";
    } else {
        std::cerr << "Impossible de cacher " << GAME_EXE << "\n";
    }

    LaunchGame(exeDir);
    return 0;
}
