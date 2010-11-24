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

#include "novel_dialog.h"

#include "data.h"

#include <QDateEdit>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>

//-----------------------------------------------------------------------------

NovelDialog::NovelDialog(QWidget* parent, Database* data) :
	QDialog(parent),
	m_data(data)
{
	setWindowTitle(tr("Edit Novel"));

	// Create name widget
	m_name = new QLineEdit(this);
	connect(m_name, SIGNAL(textChanged(const QString&)), this, SLOT(checkAcceptAllowed()));

	QHBoxLayout* name_layout = new QHBoxLayout;
	name_layout->addWidget(new QLabel(tr("Name:"), this));
	name_layout->addWidget(m_name);

	// Create goals widgets
	QGroupBox* goals = new QGroupBox(tr("Goals"), this);

	m_total = new QSpinBox(goals);
	m_total->setCorrectionMode(QSpinBox::CorrectToNearestValue);
	m_total->setRange(0, 9999999);
	m_total->setSpecialValueText(tr("N/A"));

	m_daily = new QSpinBox(goals);
	m_daily->setCorrectionMode(QSpinBox::CorrectToNearestValue);
	m_daily->setRange(0, 9999999);
	m_daily->setSpecialValueText(tr("N/A"));

	// Create date widgets
	QGroupBox* dates = new QGroupBox(tr("Date Range"), this);

	m_start = new QDateEdit(dates);
	m_start->setCalendarPopup(true);
	connect(m_start, SIGNAL(dateChanged(const QDate&)), this, SLOT(checkAcceptAllowed()));

	m_end = new QDateEdit(dates);
	m_end->setCalendarPopup(true);
	connect(m_end, SIGNAL(dateChanged(const QDate&)), this, SLOT(checkAcceptAllowed()));

	// Create buttons
	QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
	m_accept = buttons->button(QDialogButtonBox::Ok);
	connect(buttons, SIGNAL(accepted()), this, SLOT(accept()));
	connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));

	// Lay out window
	QFormLayout* goals_layout = new QFormLayout(goals);
	goals_layout->addRow(tr("Total:"), m_total);
	goals_layout->addRow(tr("Daily:"), m_daily);

	QFormLayout* dates_layout = new QFormLayout(dates);
	dates_layout->addRow(tr("Start On:"), m_start);
	dates_layout->addRow(tr("End On:"), m_end);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addLayout(name_layout);
	layout->addWidget(goals);
	layout->addWidget(dates);
	layout->addStretch();
	layout->addWidget(buttons);

	// Load values
	m_name->setText(m_data->currentNovel());
	m_total->setValue(m_data->finalGoal());
	m_daily->setValue(m_data->dailyGoal());
	m_start->setDate(m_data->startDate());
	m_end->setDate(m_data->endDate());
}

//-----------------------------------------------------------------------------

void NovelDialog::accept()
{
	if (m_data->renameNovel(m_name->text()) == false) {
		QMessageBox::warning(this, tr("Sorry"), tr("A novel already exists with that name."));
		return;
	}
	m_data->setFinalGoal(m_total->value());
	m_data->setDailyGoal(m_daily->value());
	m_data->setStart(m_start->date());
	m_data->setEnd(m_end->date());
	QDialog::accept();
}

//-----------------------------------------------------------------------------

void NovelDialog::checkAcceptAllowed()
{
	m_accept->setEnabled(!m_name->text().isEmpty() && m_start->date() <= m_end->date());
}

//-----------------------------------------------------------------------------
