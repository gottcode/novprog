#!/bin/bash

APP="NovProg2"
VERSION="2.0.5"

BUNDLE="$APP.app"
BUNDLE_FW="$BUNDLE/Contents/Frameworks"
EXE="$BUNDLE/Contents/MacOS/$APP"
EXE_FW="@executable_path/../Frameworks"
QT_FW=`qmake -query QT_INSTALL_LIBS`
QTCORE="QtCore.framework/Versions/4/QtCore"
QTGUI="QtGui.framework/Versions/4/QtGui"

strip $EXE

echo -n "Copying frameworks... "
mkdir $BUNDLE_FW
cp -R $QT_FW/QtCore.framework $BUNDLE_FW
cp -R $QT_FW/QtGui.framework $BUNDLE_FW
echo "Done"

echo -n "Setting framework names... "
install_name_tool -id $EXE_FW/$QTCORE $BUNDLE_FW/$QTCORE
install_name_tool -id $EXE_FW/$QTGUI $BUNDLE_FW/$QTGUI
echo "Done"

echo -n "Updating locations in executable... "
install_name_tool -change $QTCORE $EXE_FW/$QTCORE $EXE
install_name_tool -change $QTGUI $EXE_FW/$QTGUI $EXE
echo "Done"

echo -n "Updating locations in frameworks... "
install_name_tool -change $QTCORE $EXE_FW/$QTCORE $BUNDLE_FW/$QTGUI
echo "Done"

echo -n "Creating disk image... "
hdiutil create -quiet -ov -srcfolder $BUNDLE -format UDBZ -volname "$APP" "${APP}_$VERSION.dmg"
hdiutil internet-enable -quiet -yes "${APP}_$VERSION.dmg"
echo "Done"
