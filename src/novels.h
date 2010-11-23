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

#ifndef NOVPROG_NOVELS_H
#define NOVPROG_NOVELS_H

#include <QWidget>
class QListWidget;
class QListWidgetItem;
class QPushButton;
class Database;

class NovelsWindow : public QWidget
{
	Q_OBJECT
public:
	NovelsWindow(QWidget* parent, Database* data);

	bool add();
	void reload();

signals:
	void selected(const QString& novel);
	void hidden();

protected:
	void hideEvent(QHideEvent* event);

private slots:
	bool addClicked();
	void renameClicked();
	void removeClicked();
	void currentNovelChanged(QListWidgetItem* item);

private:
	Database* m_data;
	QListWidget* m_novels;
	QPushButton* m_add_button;
	QPushButton* m_rename_button;
	QPushButton* m_remove_button;
};

#endif // NOVPROG_NOVELS_H
