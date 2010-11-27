/************************************************************************
 *
 * Copyright (C) 2006, 2007, 2008, 2010, 2012 Graeme Gott <graeme@gottcode.org>
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
#include <QDesktopServices>
#include <QDir>
#include <QFileInfo>
#include <QSettings>

int main(int argc, char** argv)
{
	QApplication app(argc, argv);
	app.setApplicationName("NovProg");
	app.setApplicationVersion(VERSIONSTR);
	app.setOrganizationDomain("gottcode.org");
	app.setOrganizationName("GottCode");

	LocaleDialog::loadTranslator("novprog_");

	// Handle portability
	QString path = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
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
			foreach (const QString& novel, novels) {
				QFile::copy(novprog2 + novel, path + novel);
			}
		}
	}
	QDir::setCurrent(path);

	Window window;
	window.show();

	return app.exec();
}
