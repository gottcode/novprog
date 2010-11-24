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

#include "novels.h"

#include "data.h"
#include "novel_dialog.h"

#include <QHBoxLayout>
#include <QInputDialog>
#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

//-----------------------------------------------------------------------------

NovelsWindow::NovelsWindow(QWidget* parent, Database* data) :
	QWidget(parent, Qt::Dialog),
	m_data(data)
{
	setWindowTitle(tr("Select Novel"));

	m_novels = new QListWidget(this);
	connect(m_novels, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)), this, SLOT(currentNovelChanged(QListWidgetItem*)));
	connect(m_novels, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(hide()));

	m_add_button = new QPushButton(tr("New"), this);
	connect(m_add_button, SIGNAL(clicked()), this, SLOT(addClicked()));

	m_rename_button = new QPushButton(tr("Rename"), this);
	connect(m_rename_button, SIGNAL(clicked()), this, SLOT(renameClicked()));

	m_remove_button = new QPushButton(tr("Delete"), this);
	connect(m_remove_button, SIGNAL(clicked()), this, SLOT(removeClicked()));

	QPushButton* close_button = new QPushButton(tr("Close"), this);
	connect(close_button, SIGNAL(clicked()), this, SLOT(hide()));

	QVBoxLayout* button_layout = new QVBoxLayout;
	button_layout->addWidget(m_add_button);
	button_layout->addWidget(m_rename_button);
	button_layout->addWidget(m_remove_button);
	button_layout->addStretch();
	button_layout->addWidget(close_button);

	QHBoxLayout* layout = new QHBoxLayout(this);
	layout->addWidget(m_novels);
	layout->addLayout(button_layout);
}

//-----------------------------------------------------------------------------

bool NovelsWindow::add()
{
	return addClicked();
}

//-----------------------------------------------------------------------------

void NovelsWindow::reload()
{
	m_novels->clear();
	m_novels->addItems(Database::novels());
	QList<QListWidgetItem*> items = m_novels->findItems(m_data->currentNovel(), Qt::MatchExactly);
	bool list_empty = items.isEmpty();
	m_rename_button->setEnabled(!list_empty);
	m_remove_button->setEnabled(!list_empty);
	if (!list_empty) {
		m_novels->setCurrentItem(items.first());
	}
}

//-----------------------------------------------------------------------------

void NovelsWindow::hideEvent(QHideEvent* event)
{
	emit hidden();
	QWidget::hideEvent(event);
}

//-----------------------------------------------------------------------------

bool NovelsWindow::addClicked()
{
	NovelDialog add_dialog(QString(), m_data, this);
	if (add_dialog.exec() == QDialog::Accepted) {
		reload();
		emit selected(m_data->currentNovel());
		return true;
	}
	return false;
}

//-----------------------------------------------------------------------------

void NovelsWindow::renameClicked()
{
	bool ok;
	QString novel = QInputDialog::getText(this, tr("Rename Novel"), tr("Enter new novel name:"), QLineEdit::Normal, m_data->currentNovel(), &ok);
	if (ok && !novel.isEmpty() && m_data->renameNovel(novel)) {
		reload();
		emit selected(novel);
	}
}

//-----------------------------------------------------------------------------

void NovelsWindow::removeClicked()
{
	if (QMessageBox::question(this, tr("Delete Novel"), tr("Delete selected novel?"), QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Cancel) == QMessageBox::Yes) {
		if (m_data->deleteNovel()) {
			QStringList list = m_data->novels();
			if (!list.isEmpty()) {
				m_data->setCurrentNovel(list.first());
				emit selected(list.first());
			}
			reload();
		}
	}
}

//-----------------------------------------------------------------------------

void NovelsWindow::currentNovelChanged(QListWidgetItem* item)
{
	if (item) {
		QString novel = item->text();
		if (novel != m_data->currentNovel()) {
			m_data->setCurrentNovel(novel);
			emit selected(novel);
		}
	}
}

//-----------------------------------------------------------------------------
