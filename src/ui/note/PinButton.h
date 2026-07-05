#pragma once

#include "CornerButton.h"

class PinButton : public CornerButton
{
    Q_OBJECT
public:
    explicit PinButton(QWidget *parent = nullptr);
};
