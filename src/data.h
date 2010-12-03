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

#ifndef NOVPROG_DATA_H
#define NOVPROG_DATA_H

#include <QDate>
#include <QString>
#include <QStringList>

class Database : public QObject
{
public:
	Database(QObject* parent);

	enum GoalType
	{
		Daily,
		Total
	};

	static QStringList novels();

	bool addNovel(const QString& novel);
	bool renameNovel(const QString& novel);
	bool deleteNovel();

	QString currentNovel() const;
	int currentValue(GoalType type) const;
	void setCurrentNovel(const QString& novel);
	void setCurrentValue(int value);

	QDate startDate() const;
	QDate endDate() const;
	int goal(GoalType type) const;
	int minimumValue(GoalType type, const QDate& day) const;
	int maximumValue(GoalType type) const;
	int value(GoalType type, const QDate& day) const;
	void setGoal(GoalType type, int words);
	void setStart(const QDate& start);
	void setEnd(const QDate& end);

private:
	void read();
	void write();
	void resetValues();
	void updateValues();

private:
	QString m_novel;
	QDate m_start_date;
	QDate m_end_date;

	struct Data
	{
		Data() : maximum_value(0), goal(0) { }
		QList<int> values;
		QList<int> minimum_values;
		int maximum_value;
		int goal;
	} m_data[2];
};

#endif // NOVPROG_DATA_H
