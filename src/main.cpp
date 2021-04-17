/************************************************************************
 *
 * Copyright (C) 2006-2021 Graeme Gott <graeme@gottcode.org>
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

#include "locale_dialog.h"
#include "window.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QDir>
#include <QFileInfo>
#include <QSettings>
#include <QStandardPaths>

int main(int argc, char** argv)
{
#if (QT_VERSION < QT_VERSION_CHECK(6,0,0))
#if !defined(Q_OS_MAC)
	if (!qEnvironmentVariableIsSet("QT_DEVICE_PIXEL_RATIO")
			&& !qEnvironmentVariableIsSet("QT_AUTO_SCREEN_SCALE_FACTOR")
			&& !qEnvironmentVariableIsSet("QT_SCALE_FACTOR")
			&& !qEnvironmentVariableIsSet("QT_SCREEN_SCALE_FACTORS")) {
		QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	}
#endif
	QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif
	QApplication app(argc, argv);
	app.setApplicationName("NovProg");
	app.setApplicationDisplayName(Window::tr("Novel Progress"));
	app.setApplicationVersion(VERSIONSTR);
	app.setOrganizationDomain("gottcode.org");
	app.setOrganizationName("GottCode");
#if !defined(Q_OS_WIN) && !defined(Q_OS_MAC)
	app.setWindowIcon(QIcon::fromTheme("novprog", QIcon(":/novprog.png")));
	app.setDesktopFileName("novprog");
#endif

	LocaleDialog::loadTranslator("novprog_");

	QCommandLineParser parser;
	parser.setApplicationDescription(QCoreApplication::translate("main", "A wordcount graphing program"));
	parser.addHelpOption();
	parser.addVersionOption();
	parser.process(app);

	// Handle portability
	QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
	QFileInfo portable(app.applicationDirPath() + "/Data");
	if (portable.exists() && portable.isWritable()) {
		path = portable.absoluteFilePath();
		QSettings::setDefaultFormat(QSettings::IniFormat);
		QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, path + "/Settings");
	}

	// Change to novels directory
	path += "/Novels/";
	if (!QFileInfo(path).exists()) {
		QDir::home().mkpath(path);

		// Import NovProg2 files
		QString novprog2;
#if defined(Q_OS_MAC)
		novprog2 = QDir::homePath() + "/Library/Application Support/GottCode/NovProg2/";
#elif defined(Q_OS_UNIX)
		novprog2 = QDir::homePath() + "/.novprog2/";
#elif defined(Q_OS_WIN32)
		novprog2 = QDir::homePath() + "/Application Data/GottCode/NovProg2/";
#endif
		if (!novprog2.isEmpty()) {
			QDir dir(novprog2);
			QStringList novels = dir.entryList(QDir::Files);
			for (const QString& novel : novels) {
				QFile::copy(novprog2 + novel, path + novel);
			}
		}
	}
	QDir::setCurrent(path);

	Window window;
	window.show();

	return app.exec();
}
