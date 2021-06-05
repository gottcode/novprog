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

public slots:
	void accept() override;

private slots:
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
