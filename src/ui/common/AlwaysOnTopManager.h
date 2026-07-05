#pragma once

class QWidget;

class AlwaysOnTopManager
{
public:
    static void applyWindowFlags(QWidget *window, bool onTop);
};
