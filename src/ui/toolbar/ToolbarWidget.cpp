#include "ToolbarWidget.h"
#include "Settings.h"
#include "Design.h"
#include <QHBoxLayout>
#include <QToolButton>
#include <QVariant>
#include <QResizeEvent>


ToolbarWidget::ToolbarWidget(const QList<QList<ToolbarButton>> &groups, QWidget *parent)
    : QWidget(parent)
    , m_groups(groups)
{
    rebuild();
}

void ToolbarWidget::setButtonVisible(const QString &id, bool visible)
{
    if (visible)
        m_hiddenButtons.remove(id);
    else
        m_hiddenButtons.insert(id);
    applyVisibility();
    emit visibleButtonsChanged();
}

void ToolbarWidget::setButtonChecked(const QString &id, bool checked)
{
    if (auto *btn = m_buttons.value(id))
        btn->setChecked(checked);
}

bool ToolbarWidget::isButtonChecked(const QString &id) const
{
    if (auto *btn = m_buttons.value(id))
        return btn->isChecked();
    return false;
}

void ToolbarWidget::setButtonEnabled(const QString &id, bool enabled)
{
    if (auto *btn = m_buttons.value(id))
        btn->setEnabled(enabled);
}

bool ToolbarWidget::isButtonVisible(const QString &id) const
{
    return !m_hiddenButtons.contains(id);
}

void ToolbarWidget::applyVisibility()
{
    for (auto it = m_buttons.begin(); it != m_buttons.end(); ++it)
        it.value()->setVisible(!m_hiddenButtons.contains(it.key()) && !m_overflowHidden.contains(it.key()));

    bool anyPrevHasVisible = false;
    for (const auto &gw : m_groupWidgets) {
        bool hasVisible = false;
        for (auto *btn : gw.buttons) {
            QString id = btn->property("buttonId").toString();
            if (!m_hiddenButtons.contains(id) && !m_overflowHidden.contains(id)) {
                hasVisible = true;
                break;
            }
        }
        if (gw.separator)
            gw.separator->setVisible(hasVisible && anyPrevHasVisible);
        anyPrevHasVisible = anyPrevHasVisible || hasVisible;
    }
}

void ToolbarWidget::updateOverflow()
{
    m_overflowHidden.clear();

    constexpr int kPad           = 5;
    constexpr int kMargins       = 8; // 4 left + 4 right
    constexpr int kSepWidth      = 22; // 10 + 1 + 10 separator margins
    constexpr int kRightReserved = 4; // trailing spacing

    int available = width() - kMargins - kRightReserved;

    // Calculate right-aligned buttons total width (only visible groups with visible separators)
    int  rightTotal      = 0;
    bool prevRightHasVis = false;
    bool firstRightGroup = true;
    for (const auto &group : m_groups) {
        bool allRight = true;
        for (const auto &spec : group)
            if (!spec.rightAligned) {
                allRight = false;
                break;
            }
        if (!allRight)
            continue;
        bool hasVis = false;
        for (const auto &spec : group)
            if (!spec.id.isEmpty() && !m_hiddenButtons.contains(spec.id)) {
                hasVis = true;
                break;
            }
        if (hasVis && !firstRightGroup && prevRightHasVis)
            rightTotal += kSepWidth;
        if (hasVis)
            firstRightGroup = false;
        prevRightHasVis = hasVis;
        for (const auto &spec : group)
            if (!spec.id.isEmpty() && !m_hiddenButtons.contains(spec.id))
                rightTotal += m_iconSize + kPad * 2;
    }

    int leftAvailable = available - rightTotal;
    if (leftAvailable <= 0)
        leftAvailable = m_iconSize + kPad * 2; // show at least one button

    // Collect left-aligned buttons in order, tracking separators
    int  usedWidth      = 0;
    bool prevLeftHasVis = false;
    for (const auto &group : m_groups) {
        bool allRight = true;
        for (const auto &spec : group)
            if (!spec.rightAligned) {
                allRight = false;
                break;
            }
        if (allRight)
            continue;

        bool hasVis = false;
        for (const auto &spec : group)
            if (!spec.id.isEmpty() && !spec.rightAligned && !m_hiddenButtons.contains(spec.id)) {
                hasVis = true;
                break;
            }
        if (hasVis && prevLeftHasVis)
            usedWidth += kSepWidth;

        for (const auto &spec : group) {
            if (spec.id.isEmpty() || spec.rightAligned)
                continue;
            if (m_hiddenButtons.contains(spec.id))
                continue;
            int btnWidth = m_iconSize + kPad * 2;
            if (usedWidth + btnWidth > leftAvailable) {
                m_overflowHidden.insert(spec.id);
            }
            else {
                usedWidth += btnWidth;
            }
        }
        if (hasVis)
            prevLeftHasVis = true;
    }

    applyVisibility();
}

void ToolbarWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    if (!m_rebuilding)
        updateOverflow();
}

QSize ToolbarWidget::minimumSizeHint() const
{
    return QSize(0, m_iconSize + 12);
}

void ToolbarWidget::setButtons(const QList<QList<ToolbarButton>> &groups)
{
    m_groups = groups;
    rebuild();
}

void ToolbarWidget::rebuild()
{
    m_rebuilding = true;
    m_buttons.clear();
    m_groupWidgets.clear();
    m_overflowHidden.clear();

    auto *toolbarLayout = qobject_cast<QHBoxLayout *>(layout());
    if (toolbarLayout) {
        QLayoutItem *item;
        while ((item = toolbarLayout->takeAt(0)) != nullptr) {
            if (item->widget())
                item->widget()->deleteLater();
            delete item;
        }
        delete toolbarLayout;
    }

    auto *layout = new QHBoxLayout(this);
    layout->setContentsMargins(4, 0, 4, 0);
    layout->setSpacing(1);

    bool                        firstGroup = true;
    QList<QList<ToolbarButton>> rightGroups;

    for (const auto &group : m_groups) {
        if (group.isEmpty())
            continue;

        bool isRightGroup = true;
        for (const auto &spec : group)
            if (!spec.rightAligned) {
                isRightGroup = false;
                break;
            }
        if (isRightGroup) {
            rightGroups.append(group);
            continue;
        }

        // Check if this mixed group has any right-aligned items to extract
        QList<ToolbarButton> mixedRights;
        for (const auto &spec : group)
            if (spec.rightAligned)
                mixedRights.append(spec);

        GroupWidgets gw;

        if (!firstGroup) {
            auto *sepContainer = createToolbarSeparator();
            layout->addWidget(sepContainer);
            gw.separator = sepContainer;
        }
        firstGroup = false;

        for (const auto &spec : group) {
            if (spec.id.isEmpty() || spec.rightAligned)
                continue;
            auto *btn = createToolbarButton(spec);
            layout->addWidget(btn);
            m_buttons[spec.id] = btn;
            gw.buttons.append(btn);
        }

        if (!mixedRights.isEmpty())
            rightGroups.append(mixedRights);

        m_groupWidgets.append(gw);
    }

    layout->addStretch(1);

    bool firstRight = true;
    for (const auto &group : rightGroups) {
        if (group.isEmpty())
            continue;

        if (!firstRight) {
            layout->addWidget(createToolbarSeparator());
        }
        firstRight = false;

        for (const auto &spec : group) {
            if (spec.id.isEmpty())
                continue;
            auto *btn = createToolbarButton(spec);
            layout->addWidget(btn);
            m_buttons[spec.id] = btn;
        }
    }

    layout->addSpacing(4);

    QPalette pal = palette();
    pal.setColor(QPalette::Window, QColor(Design::Background));
    setAutoFillBackground(true);
    setPalette(pal);

    int toolbarHeight = m_iconSize + 12;
    setFixedHeight(toolbarHeight);
    setMinimumWidth(0);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    applyVisibility();
    updateOverflow();
    m_rebuilding = false;
}

void ToolbarWidget::setIconSize(int size)
{
    size = qBound(IconLimits::kMin, size, IconLimits::kMax);
    if (size == m_iconSize)
        return;
    m_iconSize = size;
    rebuild();
}

QWidget *ToolbarWidget::createToolbarSeparator()
{
    auto *container = new QWidget;
    auto *sepLayout = new QHBoxLayout(container);
    sepLayout->setContentsMargins(10, 0, 10, 0);
    sepLayout->setSpacing(0);
    auto *bar = new QWidget;
    bar->setFixedWidth(1);
    bar->setFixedHeight(16);
    bar->setStyleSheet(QStringLiteral("background: %1;").arg(Design::Separator));
    sepLayout->addWidget(bar);
    return container;
}

QToolButton *ToolbarWidget::createToolbarButton(const ToolbarButton &spec)
{
    constexpr int kPad = 5;
    auto         *btn  = new QToolButton;
    btn->setProperty("buttonId", spec.id);
    if (spec.iconForSize)
        btn->setIcon(spec.iconForSize(m_iconSize));
    btn->setText(spec.tooltip);
    btn->setToolTip(spec.tooltip);
    btn->setCheckable(spec.checkable);
    btn->setAutoRaise(true);
    btn->setFixedSize(m_iconSize + kPad * 2, m_iconSize + kPad * 2);
    btn->setIconSize(QSize(m_iconSize, m_iconSize));
    btn->setToolButtonStyle(Qt::ToolButtonIconOnly);
    btn->setCursor(Qt::PointingHandCursor);
    btn->installEventFilter(this);
    connect(btn, &QToolButton::clicked, this, [btn, cb = spec.callback](bool checked) {
        if (cb)
            cb(checked);
    });
    return btn;
}

bool ToolbarWidget::eventFilter(QObject *obj, QEvent *event)
{
    return QWidget::eventFilter(obj, event);
}
