#include "KeybindingsPage.h"
#include "IKeybindingConfiguration.h"
#include "ActionDefinition.h"
#include "Icons.h"
#include "Design.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QFrame>
#include <QKeyEvent>
#include <QTimer>
#include <QStyle>
#include <QScrollArea>
#include <optional>
#include <algorithm>

namespace
{

Design::Icon iconForAction(const ActionId &id)
{
    static const QList<ActionDefinition> allDefs = allActionDefinitions();
    for (const auto &def : allDefs)
        if (def.id == id)
            return def.icon;
    return static_cast<Design::Icon>(-1);
}

QLabel *sectionLabel(const QString &text)
{
    auto *label = new QLabel(text);
    QFont font  = label->font();
    font.setBold(true);
    font.setPointSize(font.pointSize() + 1);
    label->setFont(font);
    return label;
}

QString formatSequence(const QKeySequence &seq)
{
    if (seq.isEmpty())
        return QStringLiteral("—");
    return seq.toString(QKeySequence::NativeText);
}

} // namespace

KeybindingsPage::KeybindingsPage(IKeybindingConfiguration       *keybindings,
                                 QHash<ActionId, QKeySequence> *localOverrides,
                                 QWidget                        *parent)
    : QWidget(parent)
    , m_keybindings(keybindings)
    , m_localOverrides(localOverrides)
{
    auto *outer = new QVBoxLayout(this);
    outer->setContentsMargins(0, 0, 0, 0);

    auto *scrollArea = new QScrollArea;
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setStyleSheet(QStringLiteral("QScrollArea { background: transparent; }"));

    auto *content = new QWidget;
    content->setStyleSheet(QStringLiteral("background: transparent;"));
    m_layout = new QVBoxLayout(content);
    m_layout->setContentsMargins(12, 8, 12, 8);
    m_layout->setSpacing(4);

    scrollArea->setWidget(content);
    outer->addWidget(scrollArea, 1);

    m_errorLabel = new QLabel;
    m_errorLabel->setVisible(false);
    m_errorLabel->setStyleSheet(
        QStringLiteral("QLabel { color: %1; padding: 4px 12px; font-weight: bold; }").arg(Design::Danger));
    outer->addWidget(m_errorLabel);

    rebuild();
}

void KeybindingsPage::rebuild()
{
    m_errorLabel->setVisible(false);

    while (m_layout->count() > 0) {
        QLayoutItem *item = m_layout->takeAt(0);
        if (item->widget())
            item->widget()->deleteLater();
        delete item;
    }
    m_shortcutBtns.clear();
    m_recordingBtn    = nullptr;
    m_recordingAction = ActionId{-1};

    QList<ActionDefinition> actions = m_keybindings->allActions();

    // Group by category
    QHash<QString, QList<ActionDefinition>> groups;
    for (const ActionDefinition &def : actions)
        groups[def.category].append(def);

    // Sort within each category by sortOrder
    for (auto it = groups.begin(); it != groups.end(); ++it)
        std::sort(it.value().begin(), it.value().end(), [](const ActionDefinition &a, const ActionDefinition &b) {
            return a.sortOrder < b.sortOrder;
        });

    // Preferred category order
    QStringList categoryOrder = {
        QStringLiteral("Editor"),
        QStringLiteral("Note"),
        QStringLiteral("Global"),
    };

    bool first = true;
    for (const QString &cat : categoryOrder) {
        if (!groups.contains(cat))
            continue;
        if (!first)
            m_layout->addWidget(Design::horizontalLine());
        first = false;

        m_layout->addWidget(sectionLabel(cat));

        for (const ActionDefinition &def : groups[cat]) {
            auto *row = new QHBoxLayout;
            row->setSpacing(8);

            auto *iconLabel = new QLabel;
            iconLabel->setFixedSize(16, 16);
            Design::Icon icon = iconForAction(def.id);
            if (static_cast<int>(icon) >= 0)
                iconLabel->setPixmap(Icons::icon(icon, 16).pixmap(16, 16));
            row->addWidget(iconLabel);

            auto *label = new QLabel(def.label);
            row->addWidget(label, 1);

            QKeySequence currentSeq = currentShortcut(def.id);

            auto *shortcutBtn = new QPushButton(formatSequence(currentSeq));
            shortcutBtn->setFixedWidth(180);
            shortcutBtn->setCursor(Qt::PointingHandCursor);
            shortcutBtn->setStyleSheet(
                QStringLiteral("QPushButton { text-align: center; padding: 4px 8px; background: %1;"
                               "  border: 1px solid %2; border-radius: 3px; color: %3; }"
                               "QPushButton:hover { border-color: %4; }"
                               "QPushButton[recording=\"true\"] { border-color: %5; background: %6; }")
                    .arg(Design::Background,
                         Design::Border,
                         Design::Foreground,
                         Design::Accent,
                         Design::Danger,
                         Design::DangerDark));
            row->addWidget(shortcutBtn);

            auto *clearBtn = new QPushButton;
            clearBtn->setIcon(Icons::icon(Design::Icon::Backspace, 14));
            clearBtn->setFixedSize(28, 28);
            clearBtn->setToolTip(QStringLiteral("Clear shortcut"));
            clearBtn->setCursor(Qt::PointingHandCursor);
            clearBtn->setStyleSheet(Design::iconButtonStyle(Design::Danger));
            row->addWidget(clearBtn);

            auto *resetBtn = new QPushButton;
            resetBtn->setIcon(Icons::icon(Design::Icon::Reset, 14));
            resetBtn->setFixedSize(28, 28);
            resetBtn->setToolTip(QStringLiteral("Reset to default"));
            resetBtn->setCursor(Qt::PointingHandCursor);
            resetBtn->setStyleSheet(Design::iconButtonStyle());
            row->addWidget(resetBtn);

            m_layout->addLayout(row);
            m_shortcutBtns.insert(def.id, shortcutBtn);

            connect(shortcutBtn, &QPushButton::clicked, this, [this, def, shortcutBtn]() {
                stopRecording();
                m_recordingBtn    = shortcutBtn;
                m_recordingAction = def.id;
                Design::refreshStyleProperty(shortcutBtn, "recording", true);
                shortcutBtn->setText(QStringLiteral("..."));
                shortcutBtn->setFocus();
                grabKeyboard();
            });

            connect(resetBtn, &QPushButton::clicked, this, [this, def]() {
                stopRecording();
                if (m_localOverrides)
                    m_localOverrides->remove(def.id);
                if (auto *btn = m_shortcutBtns.value(def.id))
                    btn->setText(formatSequence(currentShortcut(def.id)));
                emit changed();
            });

            connect(clearBtn, &QPushButton::clicked, this, [this, def]() {
                stopRecording();
                if (m_localOverrides)
                    (*m_localOverrides)[def.id] = QKeySequence();
                if (auto *btn = m_shortcutBtns.value(def.id))
                    btn->setText(formatSequence(currentShortcut(def.id)));
                emit changed();
            });
        }
    }

    m_layout->addSpacing(8);
    m_layout->addWidget(Design::horizontalLine());

    auto *resetAllRow = new QHBoxLayout;
    resetAllRow->addStretch();
    auto *resetAllBtn = new QPushButton(QStringLiteral("Reset All to Defaults"));
    resetAllBtn->setStyleSheet(QStringLiteral("QPushButton { padding: 6px 16px; background: %1;"
                                              "  border: 1px solid %2; border-radius: 3px; color: %3; }"
                                              "QPushButton:hover { border-color: %3; background: %4; }")
                                   .arg(Design::Background, Design::Border, Design::Danger, Design::DangerDark));
    resetAllRow->addWidget(resetAllBtn);
    m_layout->addLayout(resetAllRow);

    connect(resetAllBtn, &QPushButton::clicked, this, [this]() {
        stopRecording();
        if (m_localOverrides)
            m_localOverrides->clear();
        for (auto it = m_shortcutBtns.constBegin(); it != m_shortcutBtns.constEnd(); ++it)
            it.value()->setText(formatSequence(currentShortcut(it.key())));
        emit changed();
    });
}

void KeybindingsPage::showError(const QString &msg)
{
    m_errorLabel->setText(msg);
    m_errorLabel->setVisible(true);
    QTimer::singleShot(4000, this, [this]() { m_errorLabel->setVisible(false); });
}

void KeybindingsPage::stopRecording()
{
    if (m_recordingBtn) {
        m_recordingBtn->setProperty("recording", false);
        m_recordingBtn->style()->unpolish(m_recordingBtn);
        m_recordingBtn->style()->polish(m_recordingBtn);
    }
    m_recordingBtn    = nullptr;
    m_recordingAction = ActionId{-1};
    releaseKeyboard();
}

QKeySequence KeybindingsPage::currentShortcut(ActionId id) const
{
    if (m_localOverrides && m_localOverrides->contains(id))
        return m_localOverrides->value(id);
    return m_keybindings->shortcut(id);
}

void KeybindingsPage::keyPressEvent(QKeyEvent *event)
{
    if (!m_recordingBtn || m_recordingAction.value == -1) {
        QWidget::keyPressEvent(event);
        return;
    }

    if (event->key() == Qt::Key_Escape) {
        stopRecording();
        return;
    }

    // Ignore isolated modifier presses
    switch (event->key()) {
    case Qt::Key_Control:
    case Qt::Key_Shift:
    case Qt::Key_Alt:
    case Qt::Key_Meta:
        return;
    default:
        break;
    }

    QKeySequence seq(event->key() | event->modifiers());
    QString      err = m_keybindings->validateShortcut(m_recordingAction, seq);

    if (err.isEmpty() && m_localOverrides && !seq.isEmpty()) {
        for (auto it = m_localOverrides->constBegin(); it != m_localOverrides->constEnd(); ++it) {
            if (it.key() != m_recordingAction && it.value() == seq) {
                const ActionDefinition &def = m_keybindings->actionDef(it.key());
                err = QStringLiteral("\"%1\" is already assigned to \"%2\"")
                          .arg(seq.toString(QKeySequence::NativeText), def.label);
                break;
            }
        }
    }

    if (!err.isEmpty()) {
        QKeySequence oldSeq = currentShortcut(m_recordingAction);
        m_recordingBtn->setText(formatSequence(oldSeq));
        m_recordingBtn->setStyleSheet(m_recordingBtn->styleSheet()
                                      + QStringLiteral(" QPushButton { border-color: %1; }").arg(Design::Danger));
        showError(err);
    }
    else {
        if (m_localOverrides)
            (*m_localOverrides)[m_recordingAction] = seq;
        m_recordingBtn->setText(formatSequence(seq));
    }

    stopRecording();
    emit changed();
}
