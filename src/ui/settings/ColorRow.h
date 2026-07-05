#pragma once

#include "ResettableRow.h"
#include <QColor>
#include <QStringList>

class QPushButton;

struct ColorGridResult
{
    QColor      selectedColor;
    QStringList customColors;
    bool        accepted;
};

class ColorRow : public ResettableRow
{
    Q_OBJECT
public:
    explicit ColorRow(QWidget *parent = nullptr);
    void                   setColor(const QColor &color);
    QColor                 color() const;
    static ColorGridResult openGrid(const QColor                       &currentColor,
                                    const QStringList                  &customColors,
                                    QWidget                            *parent,
                                    const QString                      &title     = QString(),
                                    std::function<void(const QColor &)> onPreview = nullptr);
signals:
    void chooseClicked();

private:
    QPushButton *m_colorBtn = nullptr;
    QColor       m_color;
};
