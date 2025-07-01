[Setup]
AppName=Mathieu Party
AppVersion=1.0.0
DefaultDirName={userdocs}\PartyGame
DefaultGroupName=Mathieu Party
OutputDir=output
OutputBaseFilename=MathieuPartySetup
Compression=lzma
SolidCompression=yes
DisableProgramGroupPage=yes

[Files]
Source: "..\MathieuParty.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\curl.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\7z.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\icon.ico"; DestDir: "{app}"; Flags: ignoreversion

[Icons]
Name: "{commondesktop}\Mathieu Party"; Filename: "{app}\MathieuParty.exe"; WorkingDir: "{app}"; IconFilename: "{app}\icon.ico"

[Run]
Filename: "{app}\MathieuParty.exe"; Description: "Lancer Mathieu Party"; Flags: nowait postinstall skipifsilent
