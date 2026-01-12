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

#include <QtWidgets/QApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QtNetwork/QLocalServer>
#include <QtNetwork/QLocalSocket>
#include "Viewer.h"
#include "VersionNumber.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    //App name for local server and command to tell main program to start a new window
    QString appName = QString("QPDFViewer%1%2").arg(MAJOR_VERSION).arg(MINOR_VERSION, 2, 10, QChar('0'));
    QString startCmd = "/<QPdfStart>\"?:|";

    //Open socket to running server
    QLocalSocket socket;
    socket.connectToServer(appName);

    //If we are connected, theres another program running, so either give it our cmd paramter or tell it create a new window
    if (socket.waitForConnected(500)) {
        if (argc > 1) {
            socket.write(argv[1]);
            socket.waitForBytesWritten();
        }
        else {
            socket.write(startCmd.toLatin1());
            socket.waitForBytesWritten();
        }
        return 0;
    }

    //Start local server under our app name
    QLocalServer server;
    QLocalServer::removeServer(appName); // Cleanup if last run crashed
    server.listen(appName);

    //Set core app params
    QCoreApplication::setApplicationName("QPDFViewer");
    QCoreApplication::setOrganizationName("David Badiei");
    QCoreApplication::setApplicationVersion(QString("%1.%2").arg(MAJOR_VERSION).arg(MINOR_VERSION, 2, 10, QChar('0')));

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
        vwr.openFile({ parser.positionalArguments().first() });
    vwr.show();

    //Signal that tells application to start a new app
    QObject::connect(&server, &QLocalServer::newConnection, [&]() {
        QLocalSocket* clientSocket = server.nextPendingConnection();
        QObject::connect(clientSocket, &QLocalSocket::readyRead, [&, clientSocket]() {
            QString filePath = clientSocket->readAll();
            //Create a new window in already existing process
            Viewer *newVwr = new Viewer();
            newVwr->setAttribute(Qt::WA_DeleteOnClose);
            if (filePath != startCmd)
                newVwr->openFile({ filePath });
            newVwr->show();
            clientSocket->disconnectFromServer();
            clientSocket->deleteLater();
            });
        });

    //Finished running app
    QCoreApplication::quit();
    return a.exec();
}
