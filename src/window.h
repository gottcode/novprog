/*
	SPDX-FileCopyrightText: 2006-2014 Graeme Gott <graeme@gottcode.org>

	SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef NOVPROG_WINDOW_H
#define NOVPROG_WINDOW_H

class Database;
class Graph;

#include <QMainWindow>
class QAction;
class QComboBox;
class QProgressBar;
class QSpinBox;
class QToolButton;

class Window : public QMainWindow
{
	Q_OBJECT

public:
	explicit Window();

protected:
	void closeEvent(QCloseEvent* event) override;

private Q_SLOTS:
	void newNovel();
	void editNovel();
	void deleteNovel();
	void load(const QString& novel);
	void novelModified();
	void wordcountEdited();
	void modifyWordCount();

private:
	void reloadList();

private:
	Database* m_data;
	Graph* m_daily_graph;
	Graph* m_total_graph;
	QComboBox* m_novels;
	QAction* m_edit_button;
	QAction* m_delete_button;
	QProgressBar* m_daily_progress;
	QProgressBar* m_total_progress;
	QSpinBox* m_wordcount;
	QToolButton* m_modify_wordcount;
};

#endif // NOVPROG_WINDOW_H
