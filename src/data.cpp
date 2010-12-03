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

#include "data.h"

#include <QDir>
#include <QSettings>

//-----------------------------------------------------------------------------

Database::Database(QObject* parent) :
	QObject(parent),
	m_daily_goal(0),
	m_final_goal(0)
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
	int value = 0;
	if (!m_values.isEmpty()) {
		value = (type == Total) ? m_values.last() : m_daily_values.last();
	}
	return value;
}

//-----------------------------------------------------------------------------

void Database::setCurrentValue(int value)
{
	if (!m_novel.isEmpty()) {
		int pos = m_start_date.daysTo(QDate::currentDate());
		if (pos >= m_values.count()) {
			int last_value = 0;
			if (m_values.count() > 0) {
				last_value = m_values.last();
			}
			int length = pos - m_values.count();
			for (int i = 0; i <= length; ++i) {
				m_values.append(last_value);
			}
		}
		m_values[pos] = value;
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
	return (type == Total ? m_final_goal : m_daily_goal);
}

//-----------------------------------------------------------------------------

int Database::minimumValue(GoalType type, const QDate& day) const
{
	return (type == Total ? m_minimum_values : m_daily_minimum_values).value(m_start_date.daysTo(day));
}

//-----------------------------------------------------------------------------

int Database::maximumValue(GoalType type) const
{
	return (type == Total ? m_maximum_value : m_daily_maximum_value);
}

//-----------------------------------------------------------------------------

int Database::value(GoalType type, const QDate& day) const
{
	return (type == Total ? m_values : m_daily_values).value(m_start_date.daysTo(day));
}

//-----------------------------------------------------------------------------

void Database::setGoal(GoalType type, int words)
{
	if (!m_novel.isEmpty()) {
		(type == Total ? m_final_goal : m_daily_goal) = words;
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
	QStringList lines = data.split('\n', QString::SkipEmptyParts);
	if (lines.count() == 0) {
		return;
	}
	QStringList header = lines.takeFirst().split(' ', QString::SkipEmptyParts);
	if (header.count() != 4) {
		return;
	}
	m_novel = novel;
	m_final_goal = header[0].toInt();
	m_daily_goal = header[1].toInt();
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

	// Parse values
	foreach (QString day, lines) {
		int pos = day.section(' ', 0, 0).toInt() - 1;
		int value = day.section(' ', 1, 1).toInt();
		if (pos >= m_values.count()) {
			int last_value = 0;
			if (m_values.count() > 0) {
				last_value = m_values.last();
			}
			int length = pos - m_values.count();
			for (int i = 0; i < length; ++i) {
				m_values.append(last_value);
			}
		}
		m_values.append(value);
	}
	updateValues();
}

//-----------------------------------------------------------------------------

void Database::write()
{
	QString data = QString("%1 %2 %3 %4\n")
		.arg(m_final_goal)
		.arg(m_daily_goal)
		.arg(m_start_date.toString(Qt::ISODate))
		.arg(m_end_date.toString(Qt::ISODate));

	int value = 0;
	QDate day = m_start_date;
	int length = m_values.count();
	for (int i = 0; i < length; ++i) {
		int new_value = m_values[i];
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
	m_values.clear();
	m_minimum_values.clear();
	m_daily_values.clear();
	m_daily_minimum_values.clear();
	m_maximum_value = 0;
	m_daily_maximum_value = 0;
	m_daily_goal = 0;
	m_final_goal = 0;
	m_start_date.setDate(0, 0, 0);
	m_end_date = m_start_date;
}

//-----------------------------------------------------------------------------

void Database::updateValues()
{
	// Calculate daily values
	m_daily_values.clear();
	m_maximum_value = 0;
	m_daily_maximum_value = 0;
	int prev_value = 0;
	foreach (int value, m_values) {
		m_maximum_value = qMax(value, m_maximum_value);
		m_daily_values.append(qMax(0, value - prev_value));
		m_daily_maximum_value = qMax(m_daily_values.last(), m_daily_maximum_value);
		prev_value = value;
	}

	// Calculate minimum values
	m_minimum_values.clear();
	m_daily_minimum_values.clear();
	int count = m_start_date.daysTo(m_end_date) + 1;
	int end = qMin(count, m_start_date.daysTo(QDate::currentDate()) + 1);
	double days = count;
	double remaining = m_final_goal;
	double delta = remaining / days;
	for (int i = 1; i <= count; ++i) {
		m_minimum_values.append(qRound(delta * i));
		m_daily_minimum_values.append(qRound(remaining / days));
		days -= 1;
		int value = m_daily_values.value(i - 1);
		if ((i >= end) && (value == 0)) {
			value = m_daily_minimum_values.last();
		}
		remaining = qMax(remaining - value, 0.0);
	}
}

//-----------------------------------------------------------------------------
