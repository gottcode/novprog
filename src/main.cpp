/************************************************************************
 *
 * Copyright (C) 2006-2008 Graeme Gott <graeme@gottcode.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include <QApplication>
#include <QDir>
#include <QFileInfo>
#include <QLocale>
#include <QSettings>
#include <QTranslator>

#include "window.h"

int main(int argc, char** argv) {
	QApplication app(argc, argv);
	app.setApplicationName("NovProg2");
	app.setOrganizationDomain("gottcode.org");
	app.setOrganizationName("GottCode");

	QTranslator qt_translator;
	qt_translator.load("qt_" + QLocale::system().name());
	app.installTranslator(&qt_translator);

	QTranslator novprog2_translator;
	novprog2_translator.load("novprog2_" + QLocale::system().name());
	app.installTranslator(&novprog2_translator);

	// Change to novels directory
#if defined(Q_OS_MAC)
	QString path = QDir::homePath() + "/Library/Application Support/GottCode/NovProg2/";
#elif defined(Q_OS_UNIX)
	QString path = QDir::homePath() + "/.novprog2/";
#elif defined(Q_OS_WIN32)
	QString path = QDir::homePath() + "/Application Data/GottCode/NovProg2/";
#endif
	path = QSettings().value("Location", path).toString();
	if (!QFileInfo(path).exists())
		QDir::home().mkpath(path);
	QDir::setCurrent(path);

	Window window;
	if (!window.isHidden()) {
		return app.exec();
	} else {
		return 0;
	}
}
