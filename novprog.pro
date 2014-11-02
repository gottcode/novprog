lessThan(QT_VERSION, 5.2) {
	error("NovProg requires Qt 5.2 or greater")
}
TEMPLATE = app
QT += widgets
CONFIG += warn_on c++11

# Allow in-tree builds
!win32 {
	MOC_DIR = build
	OBJECTS_DIR = build
	RCC_DIR = build
}

# Set program version
VERSION = 3.0.0
isEmpty(VERSION) {
	VERSION = 0
}
DEFINES += VERSIONSTR=\\\"$${VERSION}\\\"

# Set program name
unix: !macx {
	TARGET = novprog
} else {
	TARGET = NovProg
}

# Specify program sources
HEADERS = src/data.h \
	src/graph.h \
	src/locale_dialog.h \
	src/novel_dialog.h \
	src/window.h

SOURCES = src/data.cpp \
	src/graph.cpp \
	src/locale_dialog.cpp \
	src/novel_dialog.cpp \
	src/window.cpp \
	src/main.cpp

# Allow for updating translations
TRANSLATIONS = $$files(translations/novprog_*.ts)

# Install program data
RESOURCES += icons/icon-application-menu.qrc

macx {
	ICON = icons/novprog.icns
} else:win32 {
	RC_FILE = icons/icon.rc
} else:unix {
	isEmpty(PREFIX) {
		PREFIX = /usr
	}
	isEmpty(BINDIR) {
		BINDIR = $$PREFIX/bin
	}
	isEmpty(DATADIR) {
		DATADIR = $$PREFIX/share
	}

	RESOURCES += icons/icon.qrc

	target.path = $$BINDIR

	icon.files = icons/hicolor/*
	icon.path = $$DATADIR/icons/hicolor

	desktop.files = icons/novprog.desktop
	desktop.path = $$DATADIR/applications

	appdata.files = icons/novprog.appdata.xml
	appdata.path = $$DATADIR/appdata

	qm.files = translations/*.qm
	qm.path = $$DATADIR/novprog/translations

	INSTALLS += target icon desktop appdata qm
}
