#pragma once

#include <QWidget>
#include <QLineEdit>

class QToolButton;

class SearchBar : public QWidget
{
    Q_OBJECT
public:
    explicit SearchBar(QWidget *parent = nullptr);

    QString text() const
    {
        return m_input->text();
    }

signals:
    void searchRequested(const QString &text, bool forward);
    void closed();

public slots:
    void focusInput();

private:
    QLineEdit   *m_input{nullptr};
    QToolButton *m_upBtn{nullptr};
    QToolButton *m_downBtn{nullptr};
    QToolButton *m_closeBtn{nullptr};
};
