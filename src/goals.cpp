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

#include "goals.h"

#include "data.h"

#include <QDateEdit>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>

// ============================================================================

GoalsWindow::GoalsWindow(QWidget* parent, Database* data)
:	QWidget(parent, Qt::Dialog),
	m_data(data) {
	setWindowTitle(tr("Goals"));

	m_total = new QSpinBox(this);
	m_total->setCorrectionMode(QSpinBox::CorrectToNearestValue);
	m_total->setRange(0, 9999999);
	m_total->setSpecialValueText(tr("N/A"));
	connect(m_total, SIGNAL(valueChanged(int)), this, SLOT(totalEdited(int)));

	m_daily = new QSpinBox(this);
	m_daily->setCorrectionMode(QSpinBox::CorrectToNearestValue);
	m_daily->setRange(0, 9999999);
	m_daily->setSpecialValueText(tr("N/A"));
	connect(m_daily, SIGNAL(valueChanged(int)), this, SLOT(dailyEdited(int)));

	m_start = new QDateEdit(this);
	m_start->setCalendarPopup(true);
	connect(m_start, SIGNAL(dateChanged(const QDate&)), this, SLOT(startEdited(const QDate&)));

	m_end = new QDateEdit(this);
	m_end->setCalendarPopup(true);
	connect(m_end, SIGNAL(dateChanged(const QDate&)), this, SLOT(endEdited(const QDate&)));

	QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Close, Qt::Horizontal, this);
	connect(buttons, SIGNAL(rejected()), this, SLOT(hide()));

	QFormLayout* layout = new QFormLayout(this);
	layout->setSizeConstraint(QLayout::SetFixedSize);
	layout->addRow(tr("Total:"), m_total);
	layout->addRow(tr("Daily:"), m_daily);
	layout->addRow(tr("Start:"), m_start);
	layout->addRow(tr("End:"), m_end);
	layout->addRow(buttons);
}

// ============================================================================

void GoalsWindow::resetValues() {
	m_total->setValue(m_data->finalGoal());
	m_daily->setValue(m_data->dailyGoal());
	m_start->setDate(m_data->startDate());
	m_end->setDate(m_data->endDate());
}

// ============================================================================

void GoalsWindow::hideEvent(QHideEvent* event) {
	emit hidden();
	QWidget::hideEvent(event);
}

// ============================================================================

void GoalsWindow::totalEdited(int value) {
	m_data->setFinalGoal(value);
	emit modified();
}

// ============================================================================

void GoalsWindow::dailyEdited(int value) {
	m_data->setDailyGoal(value);
	emit modified();
}

// ============================================================================

void GoalsWindow::startEdited(const QDate& start) {
	m_data->setStart(start);
	emit modified();
}

// ============================================================================

void GoalsWindow::endEdited(const QDate& end) {
	m_data->setEnd(end);
	emit modified();
}

// ============================================================================
