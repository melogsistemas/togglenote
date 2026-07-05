#pragma once

#include "CornerButton.h"

class GhostIndicator : public CornerButton
{
    Q_OBJECT
public:
    explicit GhostIndicator(QWidget *parent = nullptr);
};
