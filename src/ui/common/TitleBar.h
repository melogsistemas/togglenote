#pragma once

#include <QWidget>

class QToolButton;
class QLabel;

class TitleBar : public QWidget
{
    Q_OBJECT
public:
    explicit TitleBar(QWidget *parent = nullptr);
    void setTitle(const QString &title);
    void updateMaximizeIcon();

signals:
    void closeRequested();
    void minimizeRequested();
    void maximizeRequested();

protected:
    void changeEvent(QEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;

private:
    QToolButton *m_closeBtn{nullptr};
    QToolButton *m_minimizeBtn{nullptr};
    QToolButton *m_maximizeBtn{nullptr};
    QLabel      *m_titleLabel{nullptr};
};
