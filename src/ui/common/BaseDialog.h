#pragma once

#include <QDialog>

class QVBoxLayout;
class QPushButton;
class TitleBar;
class FramelessWindow;

/// Base dialog with custom title bar, frameless window, and OK/Cancel buttons.
/// All settings/prompt dialogs should extend this for consistent look and behavior.
class BaseDialog : public QDialog
{
    Q_OBJECT
public:
    explicit BaseDialog(const QString &title,
                        QWidget       *parent        = nullptr,
                        bool           resizeEnabled = true,
                        bool           staysOnTop    = false,
                        bool           deleteOnClose = false);

    TitleBar *titleBar() const
    {
        return m_titleBar;
    }
    QPushButton *okButton() const
    {
        return m_okBtn;
    }
    QPushButton *cancelButton() const
    {
        return m_cancelBtn;
    }

protected:
    QVBoxLayout *contentLayout() const
    {
        return m_contentLayout;
    }
    void setResizeEnabled(bool enabled);
    void setupStandardButtons();

private:
    TitleBar        *m_titleBar;
    FramelessWindow *m_windowController;
    QVBoxLayout     *m_contentLayout;
    QPushButton     *m_okBtn{nullptr};
    QPushButton     *m_cancelBtn{nullptr};
};
