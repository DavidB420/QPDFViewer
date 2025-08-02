/**
 * Copyright 2025 David Badiei
 *
 * This file is part of QPDFViewer, hereafter referred to as the program.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef TABSCROLLAREA_H
#define TABSCROLLAREA_H

#include "qscrollarea.h"
#include <QScrollBar>

class TabScrollArea : public QScrollArea
{
	Q_OBJECT
public:
	TabScrollArea(QWidget* parent = nullptr);
	bool returnTopOrBottom();
protected:
	void wheelEvent(QWheelEvent* event) override;
private:
	bool topOrBottom;
signals:
	void hitExtremity();
};

#endif