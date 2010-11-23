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

#include "window.h"

#include "data.h"
#include "goals.h"
#include "graph.h"
#include "novels.h"

#include <QGridLayout>
#include <QIntValidator>
#include <QLabel>
#include <QLineEdit>
#include <QProgressBar>
#include <QPushButton>
#include <QSettings>

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
	m_graph = new Graph(this, m_data);
	m_novel_title = new QLabel(this);
	m_total_progress = new QProgressBar(this);
	m_daily_progress = new QProgressBar(this);
	m_wordcount = new QLineEdit(this);

	QValidator* validator = new QIntValidator(0, 999999, this);
	m_wordcount->setValidator(validator);
	connect(m_wordcount, SIGNAL(textEdited(const QString&)), this, SLOT(wordcountEdited(const QString&)));

	m_novels_button = new QPushButton(tr("Select"), this);
	m_novels_button->setCheckable(true);
	connect(m_novels_button, SIGNAL(toggled(bool)), this, SLOT(novelsToggled(bool)));

	m_goals_button = new QPushButton(tr("Goals"), this);
	m_goals_button->setCheckable(true);
	connect(m_goals_button, SIGNAL(toggled(bool)), this, SLOT(goalsToggled(bool)));

	QGridLayout* grid = new QGridLayout(this);
	grid->setColumnStretch(1, 1);
	grid->setSizeConstraint(QLayout::SetFixedSize);
	grid->addWidget(m_novel_title, 0, 0, 1, 2);
	grid->addWidget(m_novels_button, 0, 2);
	grid->addWidget(m_graph, 1, 0, 1, 3);
	grid->addWidget(new QLabel(tr("Total:"), this), 2, 0, Qt::AlignRight);
	grid->addWidget(m_total_progress, 2, 1, 1, 2);
	grid->addWidget(new QLabel(tr("Daily:"), this), 3, 0, Qt::AlignRight);
	grid->addWidget(m_daily_progress, 3, 1, 1, 2);
	grid->addWidget(m_wordcount, 4, 0, 1, 2);
	grid->addWidget(m_goals_button, 4, 2);

	m_novels = new NovelsWindow(this, m_data);
	connect(m_novels, SIGNAL(hidden()), this, SLOT(novelsWindowHidden()));
	connect(m_novels, SIGNAL(selected(const QString&)), this, SLOT(load(const QString&)));

	m_goals = new GoalsWindow(this, m_data);
	connect(m_goals, SIGNAL(hidden()), this, SLOT(goalsWindowHidden()));
	connect(m_goals, SIGNAL(modified()), this, SLOT(novelModified()));

	restoreGeometry(QSettings().value("Geometry").toByteArray());

	if (!m_data->currentNovel().isEmpty() || m_novels->add()) {
		load(m_data->currentNovel());
		show();
	}
}

//-----------------------------------------------------------------------------

void Window::closeEvent(QCloseEvent* event)
{
	QSettings().setValue("Geometry", saveGeometry());
	QWidget::closeEvent(event);
}

//-----------------------------------------------------------------------------

void Window::load(const QString& novel)
{
	m_data->setCurrentNovel(novel);
	m_novel_title->setText(QString("<big>%1</big>").arg(m_data->currentNovel()));
	m_wordcount->setText(QString::number(m_data->currentValue()));
	novelModified();
}

//-----------------------------------------------------------------------------

void Window::novelModified()
{
	m_graph->draw();

	// Update total progressbar
	int value = m_data->currentValue();
	m_total_progress->setRange(0, m_data->finalGoal());
	m_total_progress->setValue(qMin(m_data->finalGoal(), value));

	// Update daily progressbar
	if (value > 0) {
		QDate date = QDate::currentDate();
		int count = date.day();
		for (int i = 0; i < count; ++i) {
			date = date.addDays(-1);
			int v = m_data->value(date);
			if (v) {
				value -= v;
				break;
			}
		}
	}
	if (value < 0) {
		value = 0;
	}
	m_daily_progress->setRange(0, m_data->dailyGoal());
	m_daily_progress->setValue(qMin(m_data->dailyGoal(), value));
}

//-----------------------------------------------------------------------------

void Window::goalsToggled(bool down)
{
	if (down) {
		m_goals->resetValues();
		m_goals->show();
	} else {
		m_goals->hide();
	}
}

//-----------------------------------------------------------------------------

void Window::goalsWindowHidden()
{
	m_goals_button->setChecked(false);
}

//-----------------------------------------------------------------------------

void Window::novelsToggled(bool down)
{
	if (down) {
		m_novels->reload();
		m_novels->show();
	} else {
		m_novels->hide();
	}
}

//-----------------------------------------------------------------------------

void Window::novelsWindowHidden()
{
	m_novels_button->setChecked(false);
}

//-----------------------------------------------------------------------------

void Window::wordcountEdited(const QString& value)
{
	m_data->setCurrentValue(value.toInt());
	novelModified();
}

//-----------------------------------------------------------------------------
