#include "SettingsRowUtils.h"
#include "Icons.h"
#include "Settings.h"

#include <QSize>

namespace SettingsRowUtils
{

QPushButton *makeResetButton(QWidget *parent)
{
    auto *btn = new QPushButton(parent);
    btn->setIcon(resetIcon());
    btn->setIconSize(QSize(IconLimits::kStepButton, IconLimits::kStepButton));
    btn->setProperty("role", "reset");
    btn->setToolTip(QStringLiteral("Reset to default"));
    return btn;
}

void configureSpinbox(QSpinBox *spin)
{
    spin->setButtonSymbols(QAbstractSpinBox::NoButtons);
}

StepButtons makeStepButtons(QWidget *parent, int btnSize)
{
    auto *minus = new QPushButton(parent);
    minus->setProperty("role", "step");
    minus->setIcon(minusIcon());
    minus->setIconSize(QSize(IconLimits::kStepButton, IconLimits::kStepButton));
    minus->setFixedSize(btnSize, btnSize);
    minus->setFocusPolicy(Qt::NoFocus);
    minus->setAutoRepeat(true);

    auto *plus = new QPushButton(parent);
    plus->setProperty("role", "step");
    plus->setIcon(plusIcon());
    plus->setIconSize(QSize(IconLimits::kStepButton, IconLimits::kStepButton));
    plus->setFixedSize(btnSize, btnSize);
    plus->setFocusPolicy(Qt::NoFocus);
    plus->setAutoRepeat(true);

    return {minus, plus};
}

void addStepButtons(QSpinBox *spin, QWidget *parent, QHBoxLayout *lay)
{
    auto [minus, plus] = makeStepButtons(parent, qMax(spin->sizeHint().height(), 26));
    lay->addWidget(minus);
    lay->addWidget(plus);
    QObject::connect(minus, &QPushButton::clicked, spin, [spin]() { spin->stepBy(-1); });
    QObject::connect(plus, &QPushButton::clicked, spin, [spin]() { spin->stepBy(1); });
}

} // namespace SettingsRowUtils
