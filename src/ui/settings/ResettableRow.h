#pragma once

#include <QWidget>

class QPushButton;

class ResettableRow : public QWidget
{
    Q_OBJECT
public:
    explicit ResettableRow(QWidget *parent = nullptr);
    void setResetVisible(bool visible);
    void setResetEnabled(bool enabled);
    bool isResetEnabled() const;
signals:
    void resetClicked();

protected:
    QPushButton *resetButton() const
    {
        return m_resetBtn;
    }

private:
    QPushButton *m_resetBtn = nullptr;
};
