#!/bin/bash

APP="NovProg2"
VERSION="2.0.4"

BUNDLE="$APP.app"
EXE="$BUNDLE/Contents/MacOS/NovProg2"
EXE_FW="@executable_path/../Frameworks"
QT_FW=`qmake -query QT_INSTALL_LIBS`

# Strip executable
strip $EXE

# Create disk image without bundled Qt
echo -n "Creating disk image... "
hdiutil create -quiet -ov -srcfolder $BUNDLE -format UDBZ -volname "$APP" "$APP $VERSION.dmg"
hdiutil internet-enable -quiet -yes "$APP $VERSION.dmg"
echo "Done"

# Copy Frameworks
echo -n "Copying frameworks... "
BUNDLE_FW="$BUNDLE/Contents/Frameworks"
mkdir $BUNDLE_FW
cp -R $QT_FW/QtCore.framework $BUNDLE_FW
cp -R $QT_FW/QtGui.framework $BUNDLE_FW
echo "Done"

# Set framework names
echo -n "Setting framework names... "
QTCORE="QtCore.framework/Versions/4/QtCore"
QTGUI="QtGui.framework/Versions/4/QtGui"
install_name_tool -id $EXE_FW/$QTCORE $BUNDLE_FW/$QTCORE
install_name_tool -id $EXE_FW/$QTGUI $BUNDLE_FW/$QTGUI
echo "Done"

# Update locations in executable
echo -n "Updating locations in executable... "
install_name_tool -change $QTCORE $EXE_FW/$QTCORE $EXE
install_name_tool -change $QTGUI $EXE_FW/$QTGUI $EXE
echo "Done"

# Update locations in frameworks
echo -n "Updating locations in frameworks... "
install_name_tool -change $QTCORE $EXE_FW/$QTCORE $BUNDLE_FW/$QTGUI
echo "Done"

# Create second disk image
echo -n "Creating disk image with bundled Qt... "
hdiutil create -quiet -ov -srcfolder $BUNDLE -format UDBZ -volname "$APP" "$APP Qt $VERSION.dmg"
hdiutil internet-enable -quiet -yes "$APP Qt $VERSION.dmg"
echo "Done"
