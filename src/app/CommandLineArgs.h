#pragma once

#include <QStringList>

struct CommandLineArgs
{
    bool show          = false;
    bool hide          = false;
    bool toggle        = false;
    bool quit          = false;
    bool createNote    = false;
    bool ghostMode     = false;
    bool helpRequested = false;
};

CommandLineArgs parseCommandLineArgs(const QStringList &args);
