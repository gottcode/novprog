@ECHO OFF

SET APP=NovProg2
FOR /f %%i IN ('git rev-parse --short HEAD') DO SET VERSION=%%i

ECHO Copying executable
MKDIR %APP%
TYPE COPYING | FIND "" /V > %APP%\COPYING.txt
TYPE CREDITS | FIND "" /V > %APP%\CREDITS.txt
COPY release\%APP%.exe %APP% >nul
strip %APP%\%APP%.exe

ECHO Copying translations
SET TRANSLATIONS=%APP%\translations
MKDIR %TRANSLATIONS%
COPY translations\*.qm %TRANSLATIONS% >nul
COPY %QTDIR%\translations\qt_*.qm %TRANSLATIONS% >nul

ECHO Copying Qt libraries
COPY %QTDIR%\bin\libgcc_s_dw2-1.dll %APP% >nul
COPY %QTDIR%\bin\mingwm10.dll %APP% >nul
COPY %QTDIR%\bin\QtCore4.dll %APP% >nul
COPY %QTDIR%\bin\QtGui4.dll %APP% >nul

ECHO Creating compressed file
CD %APP%
7z a %APP%_%VERSION%.zip * >nul
CD ..
MOVE %APP%\%APP%_%VERSION%.zip . >nul

ECHO Cleaning up
RMDIR /S /Q %APP%
