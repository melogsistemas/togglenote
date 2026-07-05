#pragma once

#include "CornerButton.h"

class LockButton : public CornerButton
{
    Q_OBJECT
public:
    explicit LockButton(QWidget *parent = nullptr);
};
