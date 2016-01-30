@ECHO OFF

SET APP=NovProg
SET VERSION=3.1.0

ECHO Copying executable
MKDIR %APP%
TYPE COPYING | FIND "" /V > %APP%\COPYING.txt
TYPE CREDITS | FIND "" /V > %APP%\CREDITS.txt
TYPE README | FIND "" /V > %APP%\README.txt
COPY release\%APP%.exe %APP% >nul
strip %APP%\%APP%.exe

ECHO Copying translations
SET TRANSLATIONS=%APP%\translations
MKDIR %TRANSLATIONS%
COPY translations\*.qm %TRANSLATIONS% >nul
COPY %QTDIR%\translations\qt_*.qm %TRANSLATIONS% >nul

ECHO Copying Qt libraries
COPY %QTDIR%\bin\libgcc_s_dw2-1.dll %APP% >nul
COPY "%QTDIR%\bin\libstdc++-6.dll" %APP% >nul
COPY %QTDIR%\bin\libwinpthread-1.dll %APP% >nul
COPY %QTDIR%\bin\Qt5Core.dll %APP% >nul
COPY %QTDIR%\bin\Qt5Gui.dll %APP% >nul
COPY %QTDIR%\bin\Qt5Widgets.dll %APP% >nul

MKDIR %APP%\platforms
COPY %QTDIR%\plugins\platforms\qwindows.dll %APP%\platforms >nul

ECHO Making portable
MKDIR %APP%\Data

ECHO Creating compressed file
CD %APP%
7z a -mx=9 %APP%_%VERSION%.zip * >nul
CD ..
MOVE %APP%\%APP%_%VERSION%.zip . >nul

ECHO Cleaning up
RMDIR /S /Q %APP%
