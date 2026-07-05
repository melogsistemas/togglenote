#pragma once

class QWidget;

class ClickThroughManager
{
public:
    static void setClickThrough(QWidget *window, bool enable);
    static bool isAvailable();
};
