TEMPLATE = app
CONFIG += warn_on
macx {
	CONFIG += x86_64
}

MOC_DIR = build
OBJECTS_DIR = build
RCC_DIR = build

VERSION = $$system(git rev-parse --short HEAD)
isEmpty(VERSION) {
	VERSION = 0
}
DEFINES += VERSIONSTR=\\\"git.$${VERSION}\\\"

unix: !macx {
	TARGET = novprog2
} else {
	TARGET = NovProg2
}

HEADERS = src/data.h \
	src/graph.h \
	src/goals.h \
	src/locale_dialog.h \
	src/novels.h \
	src/window.h

SOURCES = src/data.cpp \
	src/graph.cpp \
	src/goals.cpp \
	src/locale_dialog.cpp \
	src/novels.cpp \
	src/window.cpp \
	src/main.cpp

TRANSLATIONS = translations/novprog2_en.ts \
	translations/novprog2_fr.ts

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

	target.path = $$PREFIX/bin/

	icon.path = $$PREFIX/share/icons/hicolor/48x48/apps
	icon.files = icons/novprog2.png

	desktop.path = $$PREFIX/share/applications/
	desktop.files = novprog2.desktop

	qm.files = translations/*.qm
	qm.path = $$PREFIX/share/novprog2/translations

	INSTALLS += target icon desktop qm
}
