/**
 * Copyright 2026 David Badiei
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

#include "HyperlinkObject.h"

#include <qdesktopservices.h>
#include <qurl.h>
#include <qobject.h>

HyperlinkObject::HyperlinkObject(QWidget* parent, QRectF dimSize, QString url): QPushButton(parent)
{
    //Make push button invisible and set default values
    this->setText("");
    this->setStyleSheet("QPushButton {background-color: transparent;border: none;}");

    this->url = url;
    this->setGeometry(dimSize.x(), dimSize.y(), dimSize.width(), dimSize.height());

    this->setCursor(Qt::PointingHandCursor);

    connect(this, &QPushButton::clicked, this, &HyperlinkObject::hyperlinkClicked);

    this->show();
}

void HyperlinkObject::hyperlinkClicked()
{
    //Open url in default browser
    QDesktopServices::openUrl(QUrl(url));
}