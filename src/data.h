/*
	SPDX-FileCopyrightText: 2006-2012 Graeme Gott <graeme@gottcode.org>

	SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef NOVPROG_DATA_H
#define NOVPROG_DATA_H

#include <QDate>
#include <QObject>
#include <QString>
#include <QStringList>

class Database : public QObject
{
public:
	explicit Database(QObject* parent);

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
	int startValue() const;
	int value(GoalType type, const QDate& day) const;
	void setGoal(GoalType type, int words);
	void setStart(const QDate& start);
	void setEnd(const QDate& end);
	void setStartValue(int value);

private:
	void read();
	void write();
	void resetValues();
	void updateValues();

private:
	QString m_novel;
	QDate m_start_date;
	QDate m_end_date;
	int m_start_value;

	struct Data
	{
		explicit Data()
			: maximum_value(0)
			, goal(0)
		{
		}

		QList<int> values;
		QList<int> minimum_values;
		int maximum_value;
		int goal;
	}
	m_data[2];
};

#endif // NOVPROG_DATA_H
