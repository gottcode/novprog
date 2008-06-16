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

#ifndef NOVPROG_WINDOW_H
#define NOVPROG_WINDOW_H

#include <QWidget>
class QLabel;
class QLineEdit;
class QProgressBar;
class QPushButton;
class Database;
class GoalsWindow;
class Graph;
class NovelsWindow;

class Window : public QWidget {
	Q_OBJECT
public:
	Window();

protected:
	void closeEvent(QCloseEvent* event);

private slots:
	void load(const QString& novel);
	void novelModified();
	void goalsToggled(bool down);
	void goalsWindowHidden();
	void novelsToggled(bool down);
	void novelsWindowHidden();
	void wordcountEdited(const QString& value);

private:
	Database* m_data;
	Graph* m_graph;
	QLabel* m_novel_title;
	QProgressBar* m_total_progress;
	QProgressBar* m_daily_progress;
	QLineEdit* m_wordcount;

	GoalsWindow* m_goals;
	QPushButton* m_goals_button;
	NovelsWindow* m_novels;
	QPushButton* m_novels_button;
};

#endif // NOVPROG_WINDOW_H
