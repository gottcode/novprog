/*
	SPDX-FileCopyrightText: 2010 Graeme Gott <graeme@gottcode.org>

	SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef LOCALE_DIALOG_H
#define LOCALE_DIALOG_H

#include <QDialog>
class QComboBox;

class LocaleDialog : public QDialog
{
	Q_OBJECT

public:
	explicit LocaleDialog(QWidget* parent = nullptr);

	static void loadTranslator(const QString& appname, const QString& datadir);
	static QString languageName(const QString& language);

public Q_SLOTS:
	void accept() override;

private:
	static QStringList findTranslations();

private:
	QComboBox* m_translations;

	static QString m_current;
	static QString m_path;
	static QString m_appname;
};

#endif
