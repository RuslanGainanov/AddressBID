; -- Example1.iss --
; Demonstrates copying 3 files and creating an icon.

; SEE THE DOCUMENTATION FOR DETAILS ON CREATING .ISS SCRIPT FILES!

[Setup]
AppName=AddressBID
AppVersion=1.0
DefaultDirName={pf}\AddressBID
DefaultGroupName=AddressBID
UninstallDisplayIcon={app}\AddressBID.exe
Compression=lzma2
SolidCompression=yes
; OutputDir=userdocs:Inno Setup Examples Output

[Files]
Source: "AddressBID.exe"; DestDir: "{app}"
Source: "Base2.db"; DestDir: "{app}"
Source: "D3Dcompiler_47.dll"; DestDir: "{app}"
Source: "libEGL.dll"; DestDir: "{app}"
Source: "libgcc_s_dw2-1.dll"; DestDir: "{app}"
Source: "libGLESV2.dll"; DestDir: "{app}"
Source: "libstdc++-6.dll"; DestDir: "{app}"
Source: "libwinpthread-1.dll"; DestDir: "{app}"
Source: "Log1.txt"; DestDir: "{app}"
Source: "opengl32sw.dll"; DestDir: "{app}"
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
Source: "translations\qt_ca.qm"; DestDir: "{app}\translations"
Source: "translations\qt_cs.qm"; DestDir: "{app}\translations"
Source: "translations\qt_de.qm"; DestDir: "{app}\translations"
Source: "translations\qt_en.qm"; DestDir: "{app}\translations"
Source: "translations\qt_fi.qm"; DestDir: "{app}\translations"
Source: "translations\qt_fr.qm"; DestDir: "{app}\translations"
Source: "translations\qt_he.qm"; DestDir: "{app}\translations"
Source: "translations\qt_hu.qm"; DestDir: "{app}\translations"
Source: "translations\qt_it.qm"; DestDir: "{app}\translations"
Source: "translations\qt_ja.qm"; DestDir: "{app}\translations"
Source: "translations\qt_ko.qm"; DestDir: "{app}\translations"
Source: "translations\qt_lv.qm"; DestDir: "{app}\translations"
Source: "translations\qt_ru.qm"; DestDir: "{app}\translations"
Source: "translations\qt_sk.qm"; DestDir: "{app}\translations"
Source: "translations\qt_uk.qm"; DestDir: "{app}\translations"

[Icons]
Name: "{group}\AddressBID"; Filename: "{app}\AddressBID.exe"

[Languages]
Name: "ru"; MessagesFile: "compiler:Languages\Russian.isl"

[Run]
Filename: "{app}\AddressBID.EXE";  Flags: nowait
