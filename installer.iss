[Setup]
AppName=Multimeter Software - Group 30
AppVersion=1.0
DefaultDirName={pf}\grp30-multimeter
DefaultGroupName=Multimeter - Grp 30
Compression=lzma2
SolidCompression=yes

[Files]
Source: "software\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs

[Icons]
Name: "{group}\Multimeter"; Filename: "{app}\run.bat"
