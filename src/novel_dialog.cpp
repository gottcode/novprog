/*
	SPDX-FileCopyrightText: 2006-2014 Graeme Gott <graeme@gottcode.org>

	SPDX-License-Identifier: GPL-3.0-or-later
*/

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

NovelDialog::NovelDialog(const QString& novel, Database* data, QWidget* parent)
	: QDialog(parent)
	, m_data(data)
	, m_new(novel.isEmpty())
{
	// Create name widget
	m_name = new QLineEdit(this);
	connect(m_name, &QLineEdit::textChanged, this, &NovelDialog::checkAcceptAllowed);

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
	connect(m_start, &QDateEdit::dateChanged, this, &NovelDialog::checkAcceptAllowed);

	m_end = new QDateEdit(dates);
	m_end->setCalendarPopup(true);
	connect(m_end, &QDateEdit::dateChanged, this, &NovelDialog::checkAcceptAllowed);

	// Create word count widgets
	QGroupBox* wordcount = new QGroupBox(tr("Word Count"), this);

	m_start_value = new QSpinBox(wordcount);
	m_start_value->setCorrectionMode(QSpinBox::CorrectToNearestValue);
	m_start_value->setRange(0, 9999999);

	// Create buttons
	QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
	m_accept = buttons->button(QDialogButtonBox::Ok);
	connect(buttons, &QDialogButtonBox::accepted, this, &NovelDialog::accept);
	connect(buttons, &QDialogButtonBox::rejected, this, &NovelDialog::reject);

	// Lay out window
	QFormLayout* goals_layout = new QFormLayout(goals);
	goals_layout->addRow(tr("Total:"), m_total);
	goals_layout->addRow(tr("Daily:"), m_daily);

	QFormLayout* dates_layout = new QFormLayout(dates);
	dates_layout->addRow(tr("Start On:"), m_start);
	dates_layout->addRow(tr("End On:"), m_end);

	QFormLayout* wordcount_layout = new QFormLayout(wordcount);
	wordcount_layout->addRow(tr("Start At:"), m_start_value);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addLayout(name_layout);
	layout->addWidget(goals);
	layout->addWidget(dates);
	layout->addWidget(wordcount);
	layout->addStretch();
	layout->addWidget(buttons);

	resize(sizeHint().width() * 1.25, sizeHint().height());

	// Load values
	if (m_new) {
		setWindowTitle(tr("Add Novel"));
		m_accept->setEnabled(false);
		m_total->setValue(50000);
		m_daily->setValue(2000);
		m_start_value->setValue(0);
		QDate date = QDate::currentDate();
		m_start->setDate(QDate(date.year(), date.month(), 1));
		m_end->setDate(QDate(date.year(), date.month(), date.daysInMonth()));
	} else {
		setWindowTitle(tr("Edit Novel"));
		m_name->setText(novel);
		m_total->setValue(m_data->goal(Database::Total));
		m_daily->setValue(m_data->goal(Database::Daily));
		m_start_value->setValue(m_data->startValue());
		m_start->setDate(m_data->startDate());
		m_end->setDate(m_data->endDate());
	}
}

//-----------------------------------------------------------------------------

void NovelDialog::accept()
{
	QString novel = m_name->text();
	if (novel != m_data->currentNovel()) {
		if ((m_new ? m_data->addNovel(novel) : m_data->renameNovel(novel)) == false) {
			QMessageBox::warning(this, tr("Sorry"), tr("A novel already exists with that name."));
			return;
		}
	}

	m_data->setGoal(Database::Total, m_total->value());
	m_data->setGoal(Database::Daily, m_daily->value());
	m_data->setStartValue(m_start_value->value());
	m_data->setStart(m_start->date());
	m_data->setEnd(m_end->date());
	if (m_new) {
		m_data->setCurrentValue(m_start_value->value());
	}
	QDialog::accept();
}

//-----------------------------------------------------------------------------

void NovelDialog::checkAcceptAllowed()
{
	m_accept->setEnabled(!m_name->text().isEmpty() && m_start->date() <= m_end->date());
}

//-----------------------------------------------------------------------------
