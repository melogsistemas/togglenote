#include "GhostIndicator.h"

GhostIndicator::GhostIndicator(QWidget *parent)
    : CornerButton({Design::Icon::Ghost, QStringLiteral("Ghost mode active"), {}, false, true, 2}, parent)
{}
