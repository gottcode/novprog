MKDIR NovProg2
STRIP release\NovProg2.exe
COPY release\NovProg2.exe NovProg2
COPY %QTDIR%\bin\mingwm10.dll NovProg2
COPY %QTDIR%\bin\QtCore4.dll NovProg2
COPY %QTDIR%\bin\QtGui4.dll NovProg2
