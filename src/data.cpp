/*
	SPDX-FileCopyrightText: 2006-2014 Graeme Gott <graeme@gottcode.org>

	SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "data.h"

#include <QDir>
#include <QSettings>

#include <cmath>

//-----------------------------------------------------------------------------

Database::Database(QObject* parent)
	: QObject(parent)
{
	QString novel = QSettings().value("Current").toString();
	if (novel.isEmpty()) {
		QStringList list = novels();
		if (!list.isEmpty()) {
			novel = list.first();
		}
	}
	if (!novel.isEmpty()) {
		setCurrentNovel(novel);
		if (m_novel.isEmpty()) {
			setCurrentNovel(novels().value(0));
		}
	}
}

//-----------------------------------------------------------------------------

QStringList Database::novels()
{
	return QDir().entryList(QDir::Files | QDir::NoDotAndDotDot, QDir::Name | QDir::IgnoreCase);
}

//-----------------------------------------------------------------------------

bool Database::addNovel(const QString& novel)
{
	if (novels().contains(novel)) {
		return false;
	}

	resetValues();
	QSettings().setValue("Current", novel);
	m_novel = novel;
	write();

	return true;
}

//-----------------------------------------------------------------------------

bool Database::renameNovel(const QString& novel)
{
	bool success = false;
	if (!m_novel.isEmpty()) {
		success = QDir().rename(m_novel, novel);
		if (success) {
			m_novel = novel;
		}
	}
	return success;
}

//-----------------------------------------------------------------------------

bool Database::deleteNovel()
{
	bool success = false;
	if (!m_novel.isEmpty()) {
		success = QDir().remove(m_novel);
		if (success) {
			resetValues();
		}
	}
	return success;
}

//-----------------------------------------------------------------------------

QString Database::currentNovel() const
{
	return m_novel;
}

//-----------------------------------------------------------------------------

void Database::setCurrentNovel(const QString& novel)
{
	if (novels().contains(novel)) {
		QSettings().setValue("Current", novel);
		m_novel = novel;
		read();
	} else {
		resetValues();
	}
}

//-----------------------------------------------------------------------------

int Database::currentValue(GoalType type) const
{
	const QList<int>& data = m_data[type].values;
	if (data.isEmpty()) {
		return 0;
	}

	if (type == Total) {
		return data.last();
	} else if (m_start_date.daysTo(QDate::currentDate()) < data.count()) {
		return data.last();
	} else {
		return 0;
	}
}

//-----------------------------------------------------------------------------

void Database::setCurrentValue(int value)
{
	if (!m_novel.isEmpty()) {
		int pos = m_start_date.daysTo(QDate::currentDate());
		if (pos >= m_data[Total].values.count()) {
			int last_value = m_start_value;
			if (!m_data[Total].values.isEmpty()) {
				last_value = m_data[Total].values.last();
			}
			int length = pos - m_data[Total].values.count();
			for (int i = 0; i <= length; ++i) {
				m_data[Total].values.append(last_value);
			}
		}
		m_data[Total].values[pos] = value;
		updateValues();
		write();
	}
}

//-----------------------------------------------------------------------------

QDate Database::startDate() const
{
	return m_start_date;
}

//-----------------------------------------------------------------------------

QDate Database::endDate() const
{
	return m_end_date;
}

//-----------------------------------------------------------------------------

int Database::goal(GoalType type) const
{
	return m_data[type].goal;
}

//-----------------------------------------------------------------------------

int Database::minimumValue(GoalType type, const QDate& day) const
{
	return m_data[type].minimum_values.value(m_start_date.daysTo(day));
}

//-----------------------------------------------------------------------------

int Database::maximumValue(GoalType type) const
{
	return m_data[type].maximum_value;
}

//-----------------------------------------------------------------------------

int Database::value(GoalType type, const QDate& day) const
{
	return m_data[type].values.value(m_start_date.daysTo(day));
}

//-----------------------------------------------------------------------------

int Database::startValue() const
{
	return m_start_value;
}

//-----------------------------------------------------------------------------

void Database::setGoal(GoalType type, int words)
{
	if (!m_novel.isEmpty()) {
		m_data[type].goal = words;
		updateValues();
		write();
	}
}

//-----------------------------------------------------------------------------

void Database::setStart(const QDate& start)
{
	if (!m_novel.isEmpty()) {
		m_start_date = start;
		updateValues();
		write();
	}
}

//-----------------------------------------------------------------------------

void Database::setEnd(const QDate& end)
{
	if (!m_novel.isEmpty()) {
		m_end_date = end;
		updateValues();
		write();
	}
}

//-----------------------------------------------------------------------------

void Database::setStartValue(int value)
{
	if (!m_novel.isEmpty()) {
		m_start_value = value;
		write();
	}
}

//-----------------------------------------------------------------------------

void Database::read()
{
	QString novel = m_novel;
	resetValues();

	// Read file
	QFile file(novel);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		return;
	}
	QString data = QString::fromUtf8(file.readAll());
	file.close();

	// Parse header data
#if (QT_VERSION >= QT_VERSION_CHECK(5,14,0))
	QStringList lines = data.split('\n', Qt::SkipEmptyParts);
#else
	QStringList lines = data.split('\n', QString::SkipEmptyParts);
#endif
	if (lines.count() == 0) {
		return;
	}
#if (QT_VERSION >= QT_VERSION_CHECK(5,14,0))
	const QStringList header = lines.takeFirst().split(' ', Qt::SkipEmptyParts);
#else
	const QStringList header = lines.takeFirst().split(' ', QString::SkipEmptyParts);
#endif
	if (header.count() < 4) {
		return;
	}
	m_novel = novel;
	m_data[Total].goal = header[0].toInt();
	m_data[Daily].goal = header[1].toInt();
	m_start_date = QDate::fromString(header[2], Qt::ISODate);
	int months = header[3].toInt();
	if (months == 0) {
		m_end_date = QDate::fromString(header[3], Qt::ISODate);
	} else {
		m_end_date = m_start_date.addMonths(months).addDays(-1);
	}
	if (m_end_date < m_start_date) {
		m_end_date = m_start_date;
	}
	m_start_value = (header.count() > 4) ? header[4].toInt() : 0;

	// Parse values
	for (const QString& day : lines) {
		int pos = day.section(' ', 0, 0).toInt() - 1;
		int value = day.section(' ', 1, 1).toInt();
		if (pos >= m_data[Total].values.count()) {
			int last_value = m_start_value;
			if (!m_data[Total].values.isEmpty()) {
				last_value = m_data[Total].values.last();
			}
			int length = pos - m_data[Total].values.count();
			for (int i = 0; i < length; ++i) {
				m_data[Total].values.append(last_value);
			}
		}
		m_data[Total].values.append(value);
	}
	updateValues();
}

//-----------------------------------------------------------------------------

void Database::write()
{
	QString data = QString("%1 %2 %3 %4 %5\n")
		.arg(m_data[Total].goal)
		.arg(m_data[Daily].goal)
		.arg(m_start_date.toString(Qt::ISODate))
		.arg(m_end_date.toString(Qt::ISODate))
		.arg(m_start_value);

	int value = 0;
	QDate day = m_start_date;
	int length = m_data[Total].values.count();
	for (int i = 0; i < length; ++i) {
		int new_value = m_data[Total].values[i];
		if (new_value != value) {
			value = new_value;
			data += QString("%1 %2\n").arg(i + 1).arg(value);
		}
		day = day.addDays(1);
	}

	// Write file
	QFile file(m_novel);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
		return;
	}
	file.write(data.toUtf8());
	file.close();
}

//-----------------------------------------------------------------------------

void Database::resetValues()
{
	m_novel.clear();
	m_start_date.setDate(0, 0, 0);
	m_end_date = m_start_date;
	for (int i = Daily; i <= Total; ++i) {
		m_data[i].values.clear();
		m_data[i].minimum_values.clear();
		m_data[i].maximum_value = 0;
		m_data[i].goal = 0;
	}
}

//-----------------------------------------------------------------------------

void Database::updateValues()
{
	// Calculate daily values
	m_data[Daily].values.clear();
	m_data[Daily].maximum_value = 0;
	m_data[Total].maximum_value = 0;
	int prev_value = m_start_value;
	for (int value : m_data[Total].values) {
		m_data[Total].maximum_value = std::max(value, m_data[Total].maximum_value);
		m_data[Daily].values.append(std::max(0, value - prev_value));
		m_data[Daily].maximum_value = std::max(m_data[Daily].values.last(), m_data[Daily].maximum_value);
		prev_value = value;
	}

	// Calculate minimum values
	m_data[Daily].minimum_values.clear();
	m_data[Total].minimum_values.clear();
	int count = m_start_date.daysTo(m_end_date) + 1;
	int end = std::min(count, static_cast<int>(m_start_date.daysTo(QDate::currentDate())) + 1);
	double days = count;
	double remaining = m_data[Total].goal - m_start_value;
	double delta = remaining / days;
	int start = m_start_value - ((m_start_value / 10000) * 10000);
	for (int i = 1; i <= count; ++i) {
		m_data[Total].minimum_values.append(std::round(delta * i) + start);
		m_data[Daily].minimum_values.append(std::round(remaining / days));
		days -= 1;
		int value = m_data[Daily].values.value(i - 1);
		if ((i >= end) && (value == 0)) {
			value = m_data[Daily].minimum_values.last();
		}
		remaining = std::max(remaining - value, 0.0);
	}
}

//-----------------------------------------------------------------------------
