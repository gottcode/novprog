TEMPLATE = app
greaterThan(QT_MAJOR_VERSION, 4) {
	QT += widgets
}
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
	TARGET = novprog
} else {
	TARGET = NovProg
}

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

TRANSLATIONS = $$files(translations/novprog_*.ts)

RESOURCES += icons/icon.qrc
macx {
	ICON = icons/novprog.icns
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
	icon.files = icons/novprog.png

	desktop.path = $$PREFIX/share/applications/
	desktop.files = icons/novprog.desktop

	qm.files = translations/*.qm
	qm.path = $$PREFIX/share/novprog/translations

	INSTALLS += target icon desktop qm
}
