/************************************************************************
 *
 * Copyright (C) 2006, 2007, 2008, 2010 Graeme Gott <graeme@gottcode.org>
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

#ifndef NOVPROG_WINDOW_H
#define NOVPROG_WINDOW_H

class Database;
class Graph;

#include <QWidget>
class QComboBox;
class QProgressBar;
class QPushButton;
class QSpinBox;

class Window : public QWidget
{
	Q_OBJECT
public:
	Window();

protected:
	void closeEvent(QCloseEvent* event);

private slots:
	void newNovel();
	void editNovel();
	void deleteNovel();
	void load(const QString& novel);
	void novelModified();
	void wordcountEdited();

private:
	void reloadList();

private:
	Database* m_data;
	Graph* m_daily_graph;
	Graph* m_total_graph;
	QComboBox* m_novels;
	QPushButton* m_edit_button;
	QPushButton* m_delete_button;
	QProgressBar* m_daily_progress;
	QProgressBar* m_total_progress;
	QSpinBox* m_wordcount;
};

#endif // NOVPROG_WINDOW_H
