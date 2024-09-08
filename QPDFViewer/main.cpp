/**
 * Copyright 2024 David Badiei
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

#include <QtWidgets/QApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include "Viewer.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    //Set core app params
    QCoreApplication::setApplicationName("QPDFVIewer");
    QCoreApplication::setOrganizationName("David Badiei");
    QCoreApplication::setApplicationVersion("1.01");

    //Create cmd parser
    QCommandLineParser parser;
    parser.setApplicationDescription(QCoreApplication::applicationName());
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("file", "The file to open.");
    parser.process(a);

    //Create viewer object and open file if necessary
    Viewer vwr;
    if (!parser.positionalArguments().isEmpty())
        vwr.openFile(parser.positionalArguments().first());
    vwr.show();

    //Finished running app
    QCoreApplication::quit();
    return a.exec();
}
