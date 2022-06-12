/*
	SPDX-FileCopyrightText: 2006-2012 Graeme Gott <graeme@gottcode.org>

	SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef NOVPROG_NOVEL_DIALOG_H
#define NOVPROG_NOVEL_DIALOG_H

#include <QDialog>
class Database;
class QDate;
class QDateEdit;
class QLineEdit;
class QPushButton;
class QSpinBox;

class NovelDialog : public QDialog
{
	Q_OBJECT

public:
	NovelDialog(const QString& novel, Database* data, QWidget* parent);

public Q_SLOTS:
	void accept() override;

private Q_SLOTS:
	void checkAcceptAllowed();

private:
	Database* m_data;
	QLineEdit* m_name;
	QSpinBox* m_total;
	QSpinBox* m_daily;
	QDateEdit* m_start;
	QDateEdit* m_end;
	QSpinBox* m_start_value;
	QPushButton* m_accept;
	bool m_new;
};

#endif // NOVPROG_NOVEL_DIALOG_H
