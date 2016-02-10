; -- Example1.iss --
; Demonstrates copying 3 files and creating an icon.

; SEE THE DOCUMENTATION FOR DETAILS ON CREATING .ISS SCRIPT FILES!

[Setup]
AppName=AddressBID
AppVersion=2.0
DefaultDirName={pf}\AddressBID
DefaultGroupName=AddressBID
UninstallDisplayIcon={app}\AddressBID.exe
Compression=lzma2
SolidCompression=yes
; OutputDir=userdocs:Inno Setup Examples Output

[Files]
Source: "AddressBID.exe"; DestDir: "{app}"
Source: "base1.db"; DestDir: "{app}"
Source: "libgcc_s_dw2-1.dll"; DestDir: "{app}"
Source: "libstdc++-6.dll"; DestDir: "{app}"
Source: "libwinpthread-1.dll"; DestDir: "{app}"
Source: "Qt5Concurrent.dll"; DestDir: "{app}"
Source: "Qt5Core.dll"; DestDir: "{app}"
Source: "Qt5Gui.dll"; DestDir: "{app}"
Source: "Qt5Sql.dll"; DestDir: "{app}"
Source: "Qt5Svg.dll"; DestDir: "{app}"
Source: "Qt5Widgets.dll"; DestDir: "{app}"
Source: "README.txt"; DestDir: "{app}"; Flags: isreadme
Source: "iconengines\qsvgicon.dll"; DestDir: "{app}\iconengines"
Source: "imageformats\qdds.dll"; DestDir: "{app}\imageformats"
Source: "imageformats\qgif.dll"; DestDir: "{app}\imageformats"
Source: "imageformats\qicns.dll"; DestDir: "{app}\imageformats"
Source: "imageformats\qico.dll"; DestDir: "{app}\imageformats"
Source: "imageformats\qjp2.dll"; DestDir: "{app}\imageformats"
Source: "imageformats\qjpeg.dll"; DestDir: "{app}\imageformats"
Source: "imageformats\qmng.dll"; DestDir: "{app}\imageformats"
Source: "imageformats\qsvg.dll"; DestDir: "{app}\imageformats"
Source: "imageformats\qtga.dll"; DestDir: "{app}\imageformats"
Source: "imageformats\qtiff.dll"; DestDir: "{app}\imageformats"
Source: "imageformats\qwbmp.dll"; DestDir: "{app}\imageformats"
Source: "imageformats\qwebp.dll"; DestDir: "{app}\imageformats"
Source: "platforms\qwindows.dll"; DestDir: "{app}\platforms"
Source: "sqldrivers\qsqlite.dll"; DestDir: "{app}\sqldrivers"
Source: "sqldrivers\qsqlmysql.dll"; DestDir: "{app}\sqldrivers"
Source: "sqldrivers\qsqlodbc.dll"; DestDir: "{app}\sqldrivers"
Source: "sqldrivers\qsqlpsql.dll"; DestDir: "{app}\sqldrivers"
Source: "accessible\qtaccessiblewidgets.dll"; DestDir: "{app}\accessible"
Source: "icudt52.dll"; DestDir: "{app}"
Source: "icuin52.dll"; DestDir: "{app}"
Source: "icuuc52.dll"; DestDir: "{app}"

[Icons]
Name: "{group}\AddressBID"; Filename: "{app}\AddressBID.exe"

[Languages]
Name: "ru"; MessagesFile: "compiler:Languages\Russian.isl"

[Run]
Filename: "{app}\AddressBID.EXE";  Flags: nowait