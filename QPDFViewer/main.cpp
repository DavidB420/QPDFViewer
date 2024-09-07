#include <QtWidgets/QApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include "Viewer.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setApplicationName("QPDFVIewer");
    QCoreApplication::setOrganizationName("David Badiei");
    QCoreApplication::setApplicationVersion("1.0");
    QCommandLineParser parser;
    parser.setApplicationDescription(QCoreApplication::applicationName());
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("file", "The file to open.");
    parser.process(a);
    Viewer vwr;
    if (!parser.positionalArguments().isEmpty())
        vwr.openFile(parser.positionalArguments().first());
    vwr.show();
    QCoreApplication::quit();
    return a.exec();
}
