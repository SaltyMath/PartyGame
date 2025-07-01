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

// Vérifie si un fichier existe (compatible C++98+)
bool FileExists(const std::string& path) {
    std::ifstream f(path.c_str());
    return f.good();
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

    std::ifstream file(outFile.c_str());
    if (!file) {
        std::cerr << "Erreur : ouverture de version_temp.txt impossible.\n";
        return false;
    }

    std::getline(file, output);
    file.close();
    std::remove(outFile.c_str());

    // Nettoyage
    output.erase(output.find_last_not_of(" \n\r\t") + 1);
    return true;
}

// Télécharger un zip dans le dossier du launcher
bool DownloadFileWithCurl(const std::string& url, const std::string& filename, const std::string& exeDir) {
    std::string fullPath = exeDir + "\\" + filename;
    std::string command = "curl.exe -L \"" + url + "\" -o \"" + fullPath + "\"";
    std::cout << "Téléchargement de : " << fullPath << "\n";

    int result = system(command.c_str());
    if (result != 0) {
        std::cerr << "Erreur : curl a échoué (code " << result << ").\n";
        return false;
    }

    std::ifstream in(fullPath.c_str(), std::ifstream::ate | std::ifstream::binary);
    if (!in || in.tellg() == 0) {
        std::cerr << "Erreur : fichier vide.\n";
        std::remove(fullPath.c_str());
        return false;
    }

    std::cout << "Téléchargement réussi (" << in.tellg() << " octets)\n";
    return true;
}

// Décompresser le zip avec 7z.exe
bool ExtractZip(const std::string& zipFile, const std::string& exeDir) {
    std::string fullZipPath = exeDir + "\\" + zipFile;
    std::string command = "cd /d \"" + exeDir + "\" && 7z.exe x \"" + fullZipPath + "\" -y";
    std::cout << "Décompression : " << command << "\n";
    int result = system(command.c_str());
    return (result == 0);
}

// Lancer game.exe (même dossier ou dossier game/)
void LaunchGame(const std::string& exeDir) {
    std::string primaryPath = exeDir + "\\" + GAME_EXE;
    std::string fallbackPath = exeDir + "\\game\\" + GAME_EXE;

    std::cout << "Recherche de game.exe...\n";

    if (FileExists(primaryPath)) {
        std::cout << "Lancement de : " << primaryPath << "\n";
        ShellExecuteA(NULL, "open", primaryPath.c_str(), NULL, exeDir.c_str(), SW_SHOWDEFAULT);
    }
    else if (FileExists(fallbackPath)) {
        std::cout << "Lancement de : " << fallbackPath << "\n";
        ShellExecuteA(NULL, "open", fallbackPath.c_str(), NULL, (exeDir + "\\game").c_str(), SW_SHOWDEFAULT);
    }
    else {
        std::cerr << "Erreur : game.exe introuvable\n";
    }
}

int main() {
    std::string exeDir = GetExecutableDir();
    std::cout << "=== Launcher MathieuParty ===\n";
    std::cout << "Répertoire d'installation : " << exeDir << "\n";

    std::string onlineVersion;
    if (!DownloadTextFile(VERSION_URL, onlineVersion)) {
        std::cerr << "Impossible de récupérer la version. Lancement normal...\n";
        LaunchGame(exeDir);
        return 1;
    }

    if (onlineVersion != CURRENT_VERSION) {
        std::cout << "Nouvelle version détectée : " << onlineVersion << "\n";

        if (!DownloadFileWithCurl(ZIP_URL, GAME_ZIP, exeDir)) {
            std::cerr << "Échec du téléchargement. Lancement actuel...\n";
            LaunchGame(exeDir);
            return 1;
        }

        if (!ExtractZip(GAME_ZIP, exeDir)) {
            std::cerr << "Échec de la décompression. Lancement actuel...\n";
            LaunchGame(exeDir);
            return 1;
        }

        std::remove((exeDir + "\\" + GAME_ZIP).c_str());
        std::cout << "Mise à jour appliquée.\n";
    } else {
        std::cout << "Version à jour : " << CURRENT_VERSION << "\n";
    }

    std::cout << "Lancement du jeu...\n";
    LaunchGame(exeDir);
    return 0;
}