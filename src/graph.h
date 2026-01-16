/*
	SPDX-FileCopyrightText: 2006 Graeme Gott <graeme@gottcode.org>

	SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef NOVPROG_GRAPH_H
#define NOVPROG_GRAPH_H

#include "data.h"

#include <QCoreApplication>
#include <QGraphicsRectItem>
#include <QGraphicsView>
class QGraphicsScene;

class Bar : public QGraphicsRectItem
{
	Q_DECLARE_TR_FUNCTIONS(Bar)
public:
	Bar(int x, int y, int w, int h, int value, const QDate& day, const QColor& color);

protected:
	void hoverEnterEvent(QGraphicsSceneHoverEvent* e) override;
	void hoverLeaveEvent(QGraphicsSceneHoverEvent* e) override;
};


class Graph : public QGraphicsView
{
public:
	Graph(Database* database, Database::GoalType type, QWidget* parent);

	void draw();

private:
	Database* m_data;
	Database::GoalType m_type;
	QGraphicsScene* m_scene;
};

#endif // NOVPROG_GRAPH_H
