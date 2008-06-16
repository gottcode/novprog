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

#include "goals.h"

#include "data.h"

#include <QGridLayout>
#include <QIntValidator>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

// ============================================================================

GoalsWindow::GoalsWindow(QWidget* parent, Database* data)
:	QWidget(parent, Qt::Dialog),
	m_data(data) {
	setWindowTitle(tr("Change Goals"));

	QValidator* validator = new QIntValidator(0, 999999, this);

	m_total = new QLineEdit(this);
	m_total->setMinimumWidth(140);
	m_total->setValidator(validator);
	QLabel* total_label = new QLabel(tr("Total:"), this);
	total_label->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
	connect(m_total, SIGNAL(textEdited(const QString&)), this, SLOT(totalEdited(const QString&)));

	m_daily = new QLineEdit(this);
	m_daily->setMinimumWidth(140);
	m_daily->setValidator(validator);
	QLabel* daily_label = new QLabel(tr("Daily:"), this);
	daily_label->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
	connect(m_daily, SIGNAL(textEdited(const QString&)), this, SLOT(dailyEdited(const QString&)));

	m_months = new QLineEdit(this);
	m_months->setMinimumWidth(140);
	m_months->setValidator(validator);
	QLabel* months_label = new QLabel(tr("Months:"), this);
	months_label->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
	connect(m_months, SIGNAL(textEdited(const QString&)), this, SLOT(monthsEdited(const QString&)));

	QPushButton* close = new QPushButton(tr("Close"), this);
	connect(close, SIGNAL(clicked()), this, SLOT(hide()));

	QGridLayout* grid = new QGridLayout(this);
	grid->setSizeConstraint(QLayout::SetFixedSize);
	grid->setSpacing(0);
	grid->setColumnMinimumWidth(1, 6);
	grid->setRowMinimumHeight(3, 12);
	grid->addWidget(total_label, 0, 0);
	grid->addWidget(daily_label, 1, 0);
	grid->addWidget(months_label, 2, 0);
	grid->addWidget(m_total, 0, 2, 1, 2);
	grid->addWidget(m_daily, 1, 2, 1, 2);
	grid->addWidget(m_months, 2, 2, 1, 2);
	grid->addWidget(close, 4, 3);
}

// ============================================================================

void GoalsWindow::resetValues() {
	m_total->setText(QString::number(m_data->finalGoal()));
	m_daily->setText(QString::number(m_data->dailyGoal()));
	m_months->setText(QString::number(m_data->timeFrame()));
}

// ============================================================================

void GoalsWindow::hideEvent(QHideEvent* event) {
	emit hidden();
	QWidget::hideEvent(event);
}

// ============================================================================

void GoalsWindow::totalEdited(const QString& value) {
	m_data->setFinalGoal(value.toInt());
	emit modified();
}

// ============================================================================

void GoalsWindow::dailyEdited(const QString& value) {
	m_data->setDailyGoal(value.toInt());
	emit modified();
}

// ============================================================================

void GoalsWindow::monthsEdited(const QString& value) {
	m_data->setTimeFrame(value.toInt());
	emit modified();
}

// ============================================================================
