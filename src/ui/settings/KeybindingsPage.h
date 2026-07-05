#pragma once

#include <QWidget>
#include <QHash>
#include <QString>
#include <QKeySequence>
#include "ActionId.h"

class QVBoxLayout;
class QScrollArea;
class QPushButton;
class QLabel;
class QKeyEvent;
class IKeybindingConfiguration;

class KeybindingsPage : public QWidget
{
    Q_OBJECT
public:
    explicit KeybindingsPage(IKeybindingConfiguration       *keybindings,
                             QHash<ActionId, QKeySequence> *localOverrides = nullptr,
                             QWidget                        *parent       = nullptr);

signals:
    void changed();

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    void rebuild();
    void stopRecording();
    void showError(const QString &msg);
    QKeySequence currentShortcut(ActionId id) const;

    IKeybindingConfiguration               *m_keybindings;
    QHash<ActionId, QKeySequence>          *m_localOverrides{nullptr};
    QVBoxLayout                            *m_layout{nullptr};
    QLabel                                 *m_errorLabel{nullptr};
    QHash<ActionId, QPushButton *>          m_shortcutBtns;
    QPushButton                            *m_recordingBtn{nullptr};
    ActionId                                m_recordingAction{-1};
};
