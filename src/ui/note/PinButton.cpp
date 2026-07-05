#include "PinButton.h"

PinButton::PinButton(QWidget *parent)
    : CornerButton({Design::Icon::Pin,
                    QStringLiteral("Unpinned — note will hide with Hide All"),
                    QStringLiteral("Pinned — note will not be hidden with Hide All"),
                    true,
                    false,
                    0},
                   parent)
{}
