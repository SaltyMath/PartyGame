#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <windows.h>
#include <shlwapi.h>  // Pour PathRemoveFileSpecA

#define CURRENT_VERSION "1.0.0"
#define VERSION_URL "https://raw.githubusercontent.com/SaltyMath/PartyGame/main/version.txt"
#define ZIP_URL "https://github.com/SaltyMath/PartyGame/releases/latest/download/game.zip"
#define GAME_ZIP "game.zip"
#define GAME_EXE "game.exe"

// Obtenir le dossier de l'exécutable (dossier d'installation)
std::string GetExecutableDir() {
    char path[MAX_PATH];
    GetModuleFileNameA(NULL, path, MAX_PATH);
    PathRemoveFileSpecA(path);
    return std::string(path);
}

// Télécharger un fichier texte (version.txt)
bool DownloadTextFile(const std::string& url, std::string& output, const std::string& exeDir) {
    std::string outFile = exeDir + "\\version_temp.txt";
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
    return true;
}

// Télécharger un fichier binaire (game.zip)
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

// Décompression du .zip avec 7z.exe
bool ExtractZip(const std::string& zipFile, const std::string& exeDir) {
    std::string cmd = "cd /d \"" + exeDir + "\" && 7z.exe x \"" + zipFile + "\" -y";
    int result = system(cmd.c_str());
    if (result != 0) {
        std::cerr << "Erreur : échec de décompression (code " << result << ").\n";
    }
    return result == 0;
}

// Lancer le jeu
void LaunchGame(const std::string& exeDir) {
    std::string gamePath = exeDir + "\\" + GAME_EXE;
    ShellExecuteA(NULL, "open", gamePath.c_str(), NULL, exeDir.c_str(), SW_SHOWDEFAULT);
}

int main() {
    std::string exeDir = GetExecutableDir();
    std::cout << "=== Launcher MathieuParty ===\n";
    std::cout << "Répertoire d'installation : " << exeDir << "\n";

    std::string onlineVersion;
    if (!DownloadTextFile(VERSION_URL, onlineVersion, exeDir)) {
        std::cerr << "Impossible de récupérer la version. Lancement du jeu actuel...\n";
        LaunchGame(exeDir);
        return 1;
    }

    onlineVersion.erase(onlineVersion.find_last_not_of(" \n\r\t") + 1);

    if (onlineVersion != CURRENT_VERSION) {
        std::cout << "Nouvelle version disponible : " << onlineVersion << "\n";
        if (!DownloadFileWithCurl(ZIP_URL, GAME_ZIP, exeDir)) {
            std::cerr << "Téléchargement échoué.\n";
            return 1;
        }

        if (!ExtractZip(GAME_ZIP, exeDir)) {
            std::cerr << "Décompression échouée.\n";
            return 1;
        }

        std::remove((exeDir + "\\" + GAME_ZIP).c_str());
        std::cout << "Mise à jour terminée.\n";
    } else {
        std::cout << "Version actuelle à jour.\n";
    }

    std::string gamePath = exeDir + "\\" + GAME_EXE;
    SetFileAttributesA(gamePath.c_str(), FILE_ATTRIBUTE_HIDDEN);
    LaunchGame(exeDir);
    return 0;
}
