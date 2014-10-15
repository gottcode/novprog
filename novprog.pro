TEMPLATE = app
greaterThan(QT_MAJOR_VERSION, 4) {
	QT += widgets
}
CONFIG += warn_on
QMAKE_CXXFLAGS += -std=c++11

# Allow in-tree builds
!win32 {
	MOC_DIR = build
	OBJECTS_DIR = build
	RCC_DIR = build
}

# Set program version
VERSION = $$system(git describe)
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
RESOURCES += icons/icon.qrc

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

	target.path = $$BINDIR

	icon.path = $$DATADIR/icons/hicolor/48x48/apps
	icon.files = icons/novprog.png

	desktop.path = $$DATADIR/applications
	desktop.files = icons/novprog.desktop

	appdata.files = icons/novprog.appdata.xml
	appdata.path = $$DATADIR/appdata/

	qm.files = translations/*.qm
	qm.path = $$DATADIR/novprog/translations

	INSTALLS += target icon desktop qm
}
