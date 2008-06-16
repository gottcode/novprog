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

#include "data.h"

#include <QDir>
#include <QSettings>

// ============================================================================

Database::Database(QObject* parent)
:	QObject(parent),
	m_daily_goal(0),
	m_final_goal(0),
	m_time_frame(0) {
	QString novel = QSettings().value("Current").toString();
	if (novel.isEmpty()) {
		QStringList list = novels();
		if (!list.isEmpty())
			novel = list.first();
	}
	if (!novel.isEmpty())
		setCurrentNovel(novel);
}

// ============================================================================

QStringList Database::novels() {
	return QDir().entryList(QDir::Files | QDir::NoDotAndDotDot, QDir::Name | QDir::IgnoreCase);
}

// ============================================================================

bool Database::addNovel(const QString& novel) {
	bool success = false;
	if (!novels().contains(novel)) {
		QDate date = QDate::currentDate();
		QString data = QString("50000 2000 %1-%2-01 1\n").arg(date.year()).arg(date.month(), 2, 10, QChar('0'));

		QFile file(novel);
		if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
			return false;
		file.write(data.toAscii());
		file.close();
		success = true;
	}
	return success;
}

// ============================================================================

bool Database::renameNovel(const QString& novel) {
	bool success = false;
	if (!m_novel.isEmpty()) {
		success = QDir().rename(m_novel, novel);
		if (success)
			m_novel = novel;
	}
	return success;
}

// ============================================================================

bool Database::deleteNovel() {
	bool success = false;
	if (!m_novel.isEmpty()) {
		success = QDir().remove(m_novel);
		if (success) {
			m_novel.clear();
			m_values.clear();
			m_daily_goal = 0;
			m_final_goal = 0;
			m_time_frame = 0;
			m_start_date.setDate(0, 0, 0);
		}
	}
	return success;
}

// ============================================================================

QString Database::currentNovel() const {
	return m_novel;
}

// ============================================================================

void Database::setCurrentNovel(const QString& novel) {
	if (novels().contains(novel)) {
		QSettings().setValue("Current", novel);
		m_novel = novel;
		read();
	}
}

// ============================================================================

int Database::currentValue() const {
	int value = 0;
	if (!m_values.isEmpty())
		value = m_values.last();
	return value;
}

// ============================================================================

void Database::setCurrentValue(int value) {
	if (!m_novel.isEmpty()) {
		int pos = m_start_date.daysTo(QDate::currentDate());
		if (pos >= m_values.count()) {
			int last_value = 0;
			if (m_values.count() > 0)
				last_value = m_values.last();
			int length = pos - m_values.count();
			for (int i = 0; i <= length; ++i)
				m_values.append(last_value);
		}
		m_values[pos] = value;
		write();
	}
}

// ============================================================================

QDate Database::startDate() const {
	return m_start_date;
}

// ============================================================================

QDate Database::endDate() const {
	return m_start_date.addMonths(m_time_frame);
}

// ============================================================================

int Database::value(const QDate& day) const {
	int value = 0;
	int pos = m_start_date.daysTo(day);
	if (pos < m_values.count() && pos >= 0)
		value = m_values[pos];
	return value;
}

// ============================================================================

int Database::dailyGoal() const {
	return m_daily_goal;
}

// ============================================================================

int Database::finalGoal() const {
	return m_final_goal;
}

// ============================================================================

int Database::timeFrame() const {
	return m_time_frame;
}

// ============================================================================

void Database::setDailyGoal(int words) {
	if (!m_novel.isEmpty()) {
		m_daily_goal = words;
		write();
	}
}

// ============================================================================

void Database::setFinalGoal(int words) {
	if (!m_novel.isEmpty()) {
		m_final_goal = words;
		write();
	}
}

// ============================================================================

void Database::setTimeFrame(int months) {
	if (!m_novel.isEmpty()) {
		m_time_frame = months;
		write();
	}
}

// ============================================================================

void Database::read() {
	QString novel = m_novel;

	// Reset values to null
	m_novel.clear();
	m_values.clear();
	m_daily_goal = 0;
	m_final_goal = 0;
	m_time_frame = 0;
	m_start_date.setDate(0, 0, 0);

	// Read file
	QFile file(novel);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		return;
	QString data = file.readAll();
	file.close();

	// Parse header data
	QStringList lines = data.split('\n', QString::SkipEmptyParts);
	if (lines.count() == 0)
		return;
	QStringList header = lines.takeFirst().split(' ', QString::SkipEmptyParts);
	if (header.count() != 4)
		return;
	m_novel = novel;
	m_final_goal = header[0].toInt();
	m_daily_goal = header[1].toInt();
	m_start_date = QDate::fromString(header[2], Qt::ISODate);
	m_time_frame = header[3].toInt();

	// Parse values
	foreach (QString day, lines) {
		int pos = day.section(' ', 0, 0).toInt() - 1;
		int value = day.section(' ', 1, 1).toInt();
		if (pos >= m_values.count()) {
			int last_value = 0;
			if (m_values.count() > 0)
				last_value = m_values.last();
			int length = pos - m_values.count();
			for (int i = 0; i < length; ++i)
				m_values.append(last_value);
		}
		m_values.append(value);
	}
}

// ============================================================================

void Database::write() {
	QString data = QString("%1 %2 %3 %4\n").arg(m_final_goal).arg(m_daily_goal).arg(m_start_date.toString(Qt::ISODate)).arg(m_time_frame);

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
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
		return;
	file.write(data.toAscii());
	file.close();
}

// ============================================================================
