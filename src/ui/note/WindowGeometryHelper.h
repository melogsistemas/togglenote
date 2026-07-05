#pragma once

#include <QPair>

class QWidget;
class Note;

class WindowGeometryHelper
{
public:
    static void                  applyPosition(QWidget *window, const Note &data);
    static QPair<double, double> capturePositionPercent(QWidget *window);
};
