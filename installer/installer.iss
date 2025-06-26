[Setup]
AppName=MathieuParty
AppVersion=1.0
DefaultDirName={pf}\MathieuParty
DefaultGroupName=MathieuParty
OutputBaseFilename=MathieuPartySetup
Compression=lzma
SolidCompression=yes

[Files]
Source: "..\MathieuParty.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\game.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\libcurl-x64.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\7z.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\assets\*"; DestDir: "{app}\assets"; Flags: recursesubdirs ignoreversion

[Tasks]
Name: "desktopicon"; Description: "Créer un raccourci sur le bureau"; GroupDescription: "Icônes supplémentaires :"

[Icons]
; Menu démarrer
Name: "{group}\MathieuParty"; Filename: "{app}\MathieuParty.exe"

; Bureau
Name: "{commondesktop}\MathieuParty"; Filename: "{app}\MathieuParty.exe"; Tasks: desktopicon

; Désinstalleur
Name: "{group}\Désinstaller MathieuParty"; Filename: "{uninstallexe}"
