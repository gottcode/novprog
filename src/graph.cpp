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

#include "graph.h"

#include "data.h"

#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QStyle>

// ============================================================================

Bar::Bar(int x, int y, int w, int h, int value, const QDate& day, const QColor& color)
:	QGraphicsRectItem(x, y, w, h) {
	setPen(Qt::NoPen);
	setBrush(color);
	setToolTip(QString("%1\n%L2 words").arg(day.toString("MMM d")).arg(value));
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
	setAcceptHoverEvents(true);
#else
	setAcceptsHoverEvents(true);
#endif
	setZValue(1);
}

// ============================================================================

void Bar::hoverEnterEvent(QGraphicsSceneHoverEvent* e) {
	QBrush b = brush();
	QColor c = b.color();
	c.setAlpha(128);
	b.setColor(c);
	setBrush(b);
	QGraphicsRectItem::hoverEnterEvent(e);
}

// ============================================================================

void Bar::hoverLeaveEvent(QGraphicsSceneHoverEvent* e) {
	QBrush b = brush();
	QColor c = b.color();
	c.setAlpha(255);
	b.setColor(c);
	setBrush(b);
	QGraphicsRectItem::hoverLeaveEvent(e);
}

// ============================================================================

Graph::Graph(QWidget* parent, Database* data)
:	QGraphicsView(parent),
	m_data(data) {
	m_scene = new QGraphicsScene;
	m_scene->setBackgroundBrush(Qt::white);
	setScene(m_scene);
	setMinimumSize((30 * 9) + 14, 6 * 25);
}

// ============================================================================

void Graph::draw() {
	// Remove all current items
	foreach (QGraphicsItem* item, m_scene->items()) {
		m_scene->removeItem(item);
		delete item;
	}

	// Detemine height of scene
	int rows = qMax(m_data->currentValue(), m_data->finalGoal()) / 10000;
	int goal_row = m_data->finalGoal() / 10000;
	int columns = m_data->startDate().daysTo(m_data->endDate());
	int graph_height = (rows + 1) * 25;
	int graph_width = (columns * 9) + 14;
	m_scene->setSceneRect(0, 0, graph_width, graph_height);

	// Draw lines
	QPen pen;
	pen.setCapStyle(Qt::RoundCap);
	pen.setJoinStyle(Qt::RoundJoin);
	pen.setColor(QColor(204, 204, 204));
	pen.setStyle(Qt::SolidLine);
	m_scene->addLine(QLineF(0, graph_height - 1, graph_width, graph_height - 1), pen);
	pen.setDashPattern(QVector<qreal>() << 1 << 2);
	for (int i = 1; i <= rows; ++i) {
		if (i > goal_row)
			pen.setColor(QColor(153, 204, 255));
		int y = graph_height - (i * 25);
		m_scene->addLine(QLine(0, y, graph_width, y), pen);
	}

	// Add line labels
	QFont label_font;
	label_font.setFamily("Serif");
	label_font.setPointSize(7);
	for (int i = 1; i <= rows; ++i) {
		QGraphicsTextItem* text = new QGraphicsTextItem(QString("%1K").arg(i * 10));
		if (i <= goal_row) {
			text->setDefaultTextColor(QColor(102, 102, 102));
		} else {
			text->setDefaultTextColor(QColor(153, 204, 255));
		}
		text->setFont(label_font);
		QRectF bound = text->boundingRect();
		int h = static_cast<int>(bound.height() * 0.5f);
		int w = static_cast<int>(bound.width() + 0.5f);
		if (graph_width + w > m_scene->width())
			m_scene->setSceneRect(0, 0, graph_width + w, m_scene->height());
		text->setPos(graph_width, graph_height - (i * 25) - h);
		m_scene->addItem(text);
	}

	// Draw bars
	int prev_value = 0, value, x, y, w, h;
	QColor color;
	QDate day = m_data->startDate();
	for (int c = 0; c < columns; ++c) {
		value = m_data->value(day);
		w = 8;
		h = value / 400;
		x = (c * 9) + 8;
		y = graph_height - h;
		if (value >= prev_value) {
			if (value < m_data->finalGoal()) {
				if ((value / 10000) % 2) {
					color.setRgb(150, 150, 150);
				} else {
					color.setRgb(100, 100, 100);
				}
			} else {
				color.setRgb(153, 204, 255);
			}
		} else {
			color.setRgb(255, 0, 0);
		}
		prev_value = value;
		m_scene->addItem(new Bar(x, y, w, h, value, day, color));
		day = day.addDays(1);
	}

	// Add bar labels
	day = m_data->startDate();
	int length = day.daysTo(m_data->endDate()) / 7;
	for (int i = 0; i <= length; ++i) {
		QGraphicsTextItem* text = new QGraphicsTextItem(day.toString("M/dd"));
		text->setDefaultTextColor(QColor(102, 102, 102));
		text->setFont(label_font);
		int h = static_cast<int>(text->boundingRect().height() + 0.5f);
		if (graph_height + h > m_scene->height())
			m_scene->setSceneRect(0, 0, m_scene->width(), graph_height + h);
		text->setPos(i * 63, graph_height);
		m_scene->addItem(text);
		day = day.addDays(7);
	}

	// Resize to show graph
	int frame = style()->pixelMetric(QStyle::PM_DefaultFrameWidth) * 2;
	setMinimumSize(static_cast<int>(m_scene->width()) + frame, static_cast<int>(m_scene->height()) + frame);
}

// ============================================================================
