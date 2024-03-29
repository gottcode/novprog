/*
	SPDX-FileCopyrightText: 2006-2022 Graeme Gott <graeme@gottcode.org>

	SPDX-License-Identifier: GPL-3.0-or-later
*/

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

	// Find application data
	const QString appdir = app.applicationDirPath();
	const QString datadir = QDir::cleanPath(appdir + "/" + NOVPROG_DATADIR);

	// Handle portability
	QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
#ifdef Q_OS_MAC
	const QFileInfo portable(appdir + "/../../../Data");
#else
	const QFileInfo portable(appdir + "/Data");
#endif
	if (portable.exists() && portable.isWritable()) {
		path = portable.absoluteFilePath();
		QSettings::setDefaultFormat(QSettings::IniFormat);
		QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, path + "/Settings");
	}

	// Load application language
	LocaleDialog::loadTranslator("novprog_", datadir);

	// Handle commandline
	QCommandLineParser parser;
	parser.setApplicationDescription(QCoreApplication::translate("main", "A wordcount graphing program"));
	parser.addHelpOption();
	parser.addVersionOption();
	parser.process(app);

	// Change to novels directory
	path += "/Novels/";
	if (!QFileInfo::exists(path)) {
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

	// Create main window
	Window window;
	window.show();

	return app.exec();
}
