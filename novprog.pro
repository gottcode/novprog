TEMPLATE = app
CONFIG += warn_on release
macx {
	# Uncomment the following line to compile on PowerPC Macs
	# QMAKE_MAC_SDK = /Developer/SDKs/MacOSX10.4u.sdk
	CONFIG += x86 ppc
}

MOC_DIR = build
OBJECTS_DIR = build
RCC_DIR = build

unix: !macx {
	TARGET = novprog2
} else {
	TARGET = NovProg2
}

HEADERS = src/data.h \
	src/graph.h \
	src/goals.h \
	src/novels.h \
	src/window.h

SOURCES = src/data.cpp \
	src/graph.cpp \
	src/goals.cpp \
	src/novels.cpp \
	src/window.cpp \
	src/main.cpp

macx {
	ICON = icons/novprog2.icns
}
win32 {
	RC_FILE = icons/icon.rc
}

unix: !macx {
	isEmpty(PREFIX) {
		PREFIX = /usr
	}

	binary.path = $$PREFIX/bin/
	binary.files = novprog2

	icon.path = $$PREFIX/share/icons/hicolor/48x48/apps
	icon.files = icons/novprog2.png

	desktop.path = $$PREFIX/share/applications/
	desktop.files = novprog2.desktop

	INSTALLS += binary icon desktop
}
