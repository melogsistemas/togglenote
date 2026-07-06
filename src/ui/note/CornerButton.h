#pragma once

#include <QToolButton>
#include "Design.h"

/// Base class for small corner buttons (pin, lock, ghost indicator).
/// Provides icon, tooltip, checkable state, and stack-row positioning.
class CornerButton : public QToolButton
{
    Q_OBJECT
public:
    struct Config
    {
        Design::Icon icon;
        QString      uncheckedTooltip;
        QString      checkedTooltip;
        bool         checkable                 = true;
        bool         transparentForMouseEvents = false;
        int          stackRow                  = 0;
    };

    explicit CornerButton(const Config &cfg, QWidget *parent = nullptr);

    void updateIconSize(int baseIconSize);
    void reposition();

private:
    Config m_cfg;
};
