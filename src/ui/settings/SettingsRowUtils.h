#pragma once

#include <QHBoxLayout>
#include <QObject>
#include <QPushButton>
#include <QSpinBox>

namespace SettingsRowUtils
{

struct StepButtons
{
    QPushButton *minus;
    QPushButton *plus;
};

QPushButton *makeResetButton(QWidget *parent);
void         configureSpinbox(QSpinBox *spin);
StepButtons  makeStepButtons(QWidget *parent, int btnSize);
void         addStepButtons(QSpinBox *spin, QWidget *parent, QHBoxLayout *lay);

} // namespace SettingsRowUtils
