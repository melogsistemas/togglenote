#include "CommandLineArgs.h"
#include "constants.h"

#include <QCommandLineParser>
#include <QTextStream>
#include <iostream>

static void setupNoteParser(QCommandLineParser &parser)
{
    parser.setApplicationDescription(Constants::APP_NAME + " - A sticky notes application");
    parser.addVersionOption();

    QCommandLineOption helpOpt(QStringList{"h", "help"}, "Displays this help");
    parser.addOption(helpOpt);

    QCommandLineOption newOpt(QStringList{"n", "new"}, "Open a new note");
    parser.addOption(newOpt);

    QCommandLineOption hideOpt(QStringList{"hide"}, "Hide existing pads");
    parser.addOption(hideOpt);

    QCommandLineOption showOpt(QStringList{"s", "show"}, "Show existing pads");
    parser.addOption(showOpt);

    QCommandLineOption toggleOpt(QStringList{"t", "toggle"}, "Toggle visibility of all pads");
    parser.addOption(toggleOpt);

    QCommandLineOption quitOpt(QStringList{"q", "quit"}, "Quit the application");
    parser.addOption(quitOpt);

    QCommandLineOption ghostOpt(QStringList{"g", "ghost"}, "Toggle ghost mode for all notes");
    parser.addOption(ghostOpt);
}

CommandLineArgs parseCommandLineArgs(const QStringList &args)
{
    QCommandLineParser parser;
    setupNoteParser(parser);
    parser.parse(args);

    CommandLineArgs a;

    if (parser.isSet("help")) {
        std::cout << parser.helpText().toStdString();
        a.helpRequested = true;
        return a;
    }

    if (parser.isSet("version")) {
        std::cout << qPrintable(Constants::APP_NAME) << " " << qPrintable(Constants::APP_VERSION) << "\n";
        a.helpRequested = true;
        return a;
    }
    a.show       = parser.isSet("show");
    a.hide       = parser.isSet("hide");
    a.toggle     = parser.isSet("toggle");
    a.quit       = parser.isSet("quit");
    a.createNote = parser.isSet("new");
    a.ghostMode  = parser.isSet("ghost");
    return a;
}
