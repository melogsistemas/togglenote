#include "LockButton.h"

LockButton::LockButton(QWidget *parent)
    : CornerButton({Design::Icon::Lock,
                    QStringLiteral("Lock note — click to disable editing"),
                    QStringLiteral("Unlock note — click to enable editing"),
                    true,
                    false,
                    1},
                   parent)
{}
