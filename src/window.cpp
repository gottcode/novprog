/************************************************************************
 *
 * Copyright (C) 2006, 2007, 2008, 2010, 2014 Graeme Gott <graeme@gottcode.org>
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

#include "window.h"

#include "data.h"
#include "graph.h"
#include "novel_dialog.h"

#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QProgressBar>
#include <QPushButton>
#include <QSettings>
#include <QSpinBox>
#include <QTabWidget>
#include <QVBoxLayout>

//-----------------------------------------------------------------------------

Window::Window()
{
	setWindowTitle(tr("Novel Progress"));
#if (QT_VERSION >= QT_VERSION_CHECK(4, 6, 0))
	setWindowIcon(QIcon::fromTheme("novprog", QPixmap(":/novprog.png")));
#else
	setWindowIcon(QPixmap(":/novprog.png"));
#endif

	m_data = new Database(this);
	m_novels = new QComboBox(this);
	connect(m_novels, SIGNAL(activated(const QString&)), this, SLOT(load(const QString&)));

	QTabWidget* graphs = new QTabWidget(this);

	QWidget* total_group = new QWidget(this);
	m_total_graph = new Graph(m_data, Database::Total, total_group);
	m_total_progress = new QProgressBar(total_group);
	graphs->addTab(total_group, tr("Total"));

	QWidget* daily_group = new QWidget(this);
	m_daily_graph = new Graph(m_data, Database::Daily, daily_group);
	m_daily_progress = new QProgressBar(daily_group);
	graphs->addTab(daily_group, tr("Daily"));

	m_wordcount = new QSpinBox(this);
	m_wordcount->setCorrectionMode(QSpinBox::CorrectToNearestValue);
	m_wordcount->setRange(0, 9999999);
	m_wordcount->setFocus();
	connect(m_wordcount, SIGNAL(editingFinished()), this, SLOT(wordcountEdited()));

	QPushButton* new_button = new QPushButton(tr("New"), this);
	connect(new_button, SIGNAL(clicked()), this, SLOT(newNovel()));

	m_edit_button = new QPushButton(tr("Edit"), this);
	connect(m_edit_button, SIGNAL(clicked()), this, SLOT(editNovel()));

	m_delete_button = new QPushButton(tr("Delete"), this);
	connect(m_delete_button, SIGNAL(clicked()), this, SLOT(deleteNovel()));

	QHBoxLayout* selector_layout = new QHBoxLayout;
	selector_layout->addWidget(m_novels, 1);
	selector_layout->addWidget(new_button);
	selector_layout->addWidget(m_edit_button);
	selector_layout->addWidget(m_delete_button);

	QVBoxLayout* daily_layout = new QVBoxLayout(daily_group);
	daily_layout->addWidget(m_daily_graph);
	daily_layout->addWidget(m_daily_progress);

	QVBoxLayout* total_layout = new QVBoxLayout(total_group);
	total_layout->addWidget(m_total_graph);
	total_layout->addWidget(m_total_progress);

	QGridLayout* layout = new QGridLayout(this);
	layout->setColumnStretch(0, 1);
	layout->setColumnStretch(1, 1);
	layout->addLayout(selector_layout, 0, 0, 1, 2);
	layout->addWidget(m_wordcount, 1, 0, 1, 2, Qt::AlignCenter);
	layout->addWidget(graphs, 2, 0, 1, 2);

	restoreGeometry(QSettings().value("Geometry").toByteArray());

	reloadList();
	if (m_data->currentNovel().isEmpty()) {
		newNovel();
	}
	load(m_data->currentNovel());
}

//-----------------------------------------------------------------------------

void Window::closeEvent(QCloseEvent* event)
{
	QSettings().setValue("Geometry", saveGeometry());
	QWidget::closeEvent(event);
}

//-----------------------------------------------------------------------------

void Window::newNovel()
{
	NovelDialog add_dialog(QString(), m_data, this);
	if (add_dialog.exec() == QDialog::Accepted) {
		reloadList();
		load(m_data->currentNovel());
	}
}

//-----------------------------------------------------------------------------

void Window::editNovel()
{
	NovelDialog edit_dialog(m_data->currentNovel(), m_data, this);
	if (edit_dialog.exec() == QDialog::Accepted) {
		reloadList();
		novelModified();
	}
}

//-----------------------------------------------------------------------------

void Window::deleteNovel()
{
	if (QMessageBox::question(this, tr("Question"), tr("Delete current novel?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes) {
		if (m_data->deleteNovel()) {
			QStringList list = m_data->novels();
			if (!list.isEmpty()) {
				m_data->setCurrentNovel(list.first());
			}
			reloadList();
			load(m_data->currentNovel());
		}
	}
}

//-----------------------------------------------------------------------------

void Window::load(const QString& novel)
{
	m_data->setCurrentNovel(novel);
	m_wordcount->setValue(m_data->currentValue(Database::Total));
	novelModified();
}

//-----------------------------------------------------------------------------

void Window::novelModified()
{
	m_total_graph->draw();
	m_daily_graph->draw();

	// Update total progressbar
	int goal = m_data->goal(Database::Total);
	int value = m_data->currentValue(Database::Total);
	m_total_progress->setMaximum(qMax(1, goal));
	m_total_progress->setValue(qMin(value, goal));

	// Update daily progressbar
	goal = m_data->goal(Database::Daily);
	value = m_data->currentValue(Database::Daily);
	m_daily_progress->setMaximum(qMax(1, goal));
	m_daily_progress->setValue(qMin(value, goal));
}

//-----------------------------------------------------------------------------

void Window::wordcountEdited()
{
	int value = m_wordcount->value();
	if (value != m_data->currentValue(Database::Total)) {
		m_data->setCurrentValue(value);
		novelModified();
	}
}

//-----------------------------------------------------------------------------

void Window::reloadList()
{
	m_novels->clear();
	m_novels->addItems(Database::novels());
	int index = m_novels->findText(m_data->currentNovel());
	bool found = (index != -1);
	m_edit_button->setEnabled(found);
	m_delete_button->setEnabled(found);
	m_wordcount->setEnabled(found);
	if (found) {
		m_novels->setCurrentIndex(index);
	}
}

//-----------------------------------------------------------------------------
