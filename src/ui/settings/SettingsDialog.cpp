#include "SettingsDialog.h"
#include "SettingsViewModel.h"
#include "KeybindingsPage.h"
#include "SettingsRowUtils.h"
#include "ActionDefinition.h"
#include "ActionId.h"
#include "Icons.h"
#include "Design.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QPushButton>
#include <QCheckBox>
#include <QSpinBox>
#include <QLabel>
#include <QFrame>
#include <QScrollArea>
#include <QTabWidget>

namespace
{
const ActionDefinition *findActionDef(const QString &id)
{
    ActionId actionId = ActionId::fromString(id);
    if (actionId.value == -1)
        return nullptr;
    static const QList<ActionDefinition> allDefs = allActionDefinitions();
    for (const auto &def : allDefs)
        if (def.id == actionId)
            return &def;
    return nullptr;
}

QListWidgetItem *makeSeparatorListItem(const QString &roleId, const QString &displayText)
{
    auto *item = new QListWidgetItem(displayText);
    item->setData(Qt::UserRole, roleId);
    item->setForeground(QColor(Design::Separator));
    QFont f = item->font();
    f.setPointSize(9);
    item->setFont(f);
    return item;
}
} // namespace


SettingsDialog::SettingsDialog(SettingsViewModel *viewModel, QWidget *parent)
    : BaseDialog(QStringLiteral("Preferences"), parent)
    , m_viewModel(viewModel)
{
    const Settings &s   = m_viewModel->settings();
    m_selectedColor     = s.bgColor();
    m_selectedFontColor = s.textColor();
    m_selectedOpacity   = s.opacity();
    m_customColors      = s.customColors();
    m_toolbarLayout     = s.toolbarLayout();
    m_toolbarLayoutVis  = s.toolbarLayoutVisibility();

    setupUI();

    m_fontFamilyRow->setCurrentFont(QFont(s.fontFamily()));
    m_fontSizeRow->setValue(s.fontSize());
    m_iconSizeRow->setValue(s.iconSize());
    m_autostartCheck->setChecked(s.autostartNewNote());
    m_startHiddenCheck->setChecked(s.startHidden());
    m_defaultOnTopCheck->setChecked(s.defaultOnTop());
    m_forceXcbCheck->setChecked(s.forceXcbOnWayland());
    m_autohideCheck->setChecked(s.autohideToolbar());
    m_timeoutSpin->setValue(s.autohideTimeout());
    m_opacityRow->setValue(s.opacity());
    m_pinHoverOpacityRow->setValue(s.pinHoverOpacity());
    m_pinIdleOpacityRow->setValue(s.pinIdleOpacity());

    m_fontColorRow->setColor(m_selectedFontColor);
}

void SettingsDialog::accept()
{
    saveToolbarLayout();
    m_viewModel->setSettings(settings());
    m_viewModel->accept();
    QDialog::accept();
}

void SettingsDialog::emitChanged()
{
    saveToolbarLayout();
    m_viewModel->setSettings(settings());
    emit changed(m_viewModel->settings());
}

static QScrollArea *makeTabScroll(QWidget *parent)
{
    auto *scroll = new QScrollArea;
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    auto *tab = new QWidget;
    tab->setAutoFillBackground(true);
    QPalette pal = tab->palette();
    pal.setColor(QPalette::Window, QColor(Design::Background));
    tab->setPalette(pal);
    scroll->setWidget(tab);
    return scroll;
}

static QVBoxLayout *tabContentLayout(QScrollArea *scroll)
{
    auto *tab    = scroll->widget();
    auto *layout = new QVBoxLayout(tab);
    layout->setContentsMargins(12, 12, 12, 12);
    layout->setSpacing(8);
    return layout;
}

void SettingsDialog::setupUI()
{
    m_tabWidget = new QTabWidget;

    setupAppearanceTab();
    setupToolbarTab();
    setupStartupTab();
    setupShortcutsTab();

    contentLayout()->addWidget(m_tabWidget, 1);
    setupStandardButtons();
    setMinimumSize(500, 400);
    resize(540, 520);
}

void SettingsDialog::setupAppearanceTab()
{
    auto *scroll = makeTabScroll(m_tabWidget);
    auto *layout = tabContentLayout(scroll);
    auto *form   = new QFormLayout;
    form->setSpacing(6);
    form->setLabelAlignment(Qt::AlignRight);

    m_bgColorRow = new ColorRow(scroll->widget());
    m_bgColorRow->setColor(m_selectedColor);
    form->addRow(QStringLiteral("Bg color:"), m_bgColorRow);

    connect(m_bgColorRow, &ColorRow::chooseClicked, this, [this]() {
        auto result = ColorRow::openGrid(
            m_selectedColor, m_customColors, this, QStringLiteral("Default note color"), [this](const QColor &color) {
                m_selectedColor = color;
                m_bgColorRow->setColor(color);
                emitChanged();
            });
        m_selectedColor = result.selectedColor;
        m_bgColorRow->setColor(m_selectedColor);
        m_customColors = result.customColors;
        if (!result.accepted)
            emitChanged();
    });

    connect(m_bgColorRow, &ColorRow::resetClicked, this, [this]() {
        m_selectedColor = QColor(QStringLiteral("#ffee99"));
        m_bgColorRow->setColor(m_selectedColor);
        emitChanged();
    });

    m_fontColorRow = new ColorRow(scroll->widget());
    m_fontColorRow->setColor(m_selectedFontColor);
    form->addRow(QStringLiteral("Font color:"), m_fontColorRow);

    connect(m_fontColorRow, &ColorRow::chooseClicked, this, [this]() {
        auto result         = ColorRow::openGrid(m_selectedFontColor,
                                         m_customColors,
                                         this,
                                         QStringLiteral("Default font color"),
                                         [this](const QColor &color) {
                                             m_selectedFontColor = color;
                                             m_fontColorRow->setColor(color);
                                             emitChanged();
                                         });
        m_selectedFontColor = result.selectedColor;
        m_fontColorRow->setColor(m_selectedFontColor);
        m_customColors = result.customColors;
        if (!result.accepted)
            emitChanged();
    });

    connect(m_fontColorRow, &ColorRow::resetClicked, this, [this]() {
        m_selectedFontColor = Qt::black;
        m_fontColorRow->setColor(m_selectedFontColor);
        emitChanged();
    });

    m_fontFamilyRow = new FontFamilyRow(scroll->widget());
    m_fontFamilyRow->setResetVisible(false);
    form->addRow(QStringLiteral("Font family:"), m_fontFamilyRow);
    connect(m_fontFamilyRow, &FontFamilyRow::fontChanged, this, &SettingsDialog::emitChanged);

    m_fontSizeRow = new SpinRow(scroll->widget());
    m_fontSizeRow->setRange(FontSizeLimits::kMin, FontSizeLimits::kMax);
    m_fontSizeRow->setSuffix(QStringLiteral(" pt"));
    m_fontSizeRow->setResetVisible(false);
    form->addRow(QStringLiteral("Font size:"), m_fontSizeRow);
    connect(m_fontSizeRow, &SpinRow::valueChanged, this, &SettingsDialog::emitChanged);

    m_iconSizeRow = new SpinRow(scroll->widget());
    m_iconSizeRow->setRange(IconLimits::kMin, IconLimits::kMax);
    m_iconSizeRow->setSuffix(QStringLiteral(" px"));
    m_iconSizeRow->setToolTip(QStringLiteral("Toolbar icon size in pixels"));
    m_iconSizeRow->setResetVisible(false);
    form->addRow(QStringLiteral("Icon size:"), m_iconSizeRow);
    connect(m_iconSizeRow, &SpinRow::valueChanged, this, &SettingsDialog::emitChanged);

    m_opacityRow = new SliderSpinRow(scroll->widget());
    m_opacityRow->setRange(10, 100);
    m_opacityRow->setSuffix(QStringLiteral("%"));
    m_opacityRow->setValue(m_selectedOpacity);
    m_opacityRow->setResetVisible(false);
    form->addRow(QStringLiteral("Opacity:"), m_opacityRow);
    connect(m_opacityRow, &SliderSpinRow::valueChanged, this, [this](int value) {
        m_selectedOpacity = value;
        emitChanged();
    });

    m_pinHoverOpacityRow = new SliderSpinRow(scroll->widget());
    m_pinHoverOpacityRow->setRange(10, 100);
    m_pinHoverOpacityRow->setSuffix(QStringLiteral("%"));
    m_pinHoverOpacityRow->setToolTip(QStringLiteral("Opacity when hovering over a pinned note (solo mode)"));
    m_pinHoverOpacityRow->setResetVisible(false);
    form->addRow(QStringLiteral("Pin hover opacity:"), m_pinHoverOpacityRow);
    connect(m_pinHoverOpacityRow, &SliderSpinRow::valueChanged, this, &SettingsDialog::emitChanged);

    m_pinIdleOpacityRow = new SliderSpinRow(scroll->widget());
    m_pinIdleOpacityRow->setRange(5, 100);
    m_pinIdleOpacityRow->setSuffix(QStringLiteral("%"));
    m_pinIdleOpacityRow->setToolTip(QStringLiteral("Opacity when not hovering over a pinned note (solo mode)"));
    m_pinIdleOpacityRow->setResetVisible(false);
    form->addRow(QStringLiteral("Pin idle opacity:"), m_pinIdleOpacityRow);
    connect(m_pinIdleOpacityRow, &SliderSpinRow::valueChanged, this, &SettingsDialog::emitChanged);

    layout->addLayout(form);
    layout->addStretch();
    m_tabWidget->addTab(scroll, QStringLiteral("Appearance"));
}

void SettingsDialog::setupToolbarTab()
{
    auto *scroll    = makeTabScroll(m_tabWidget);
    auto *tabLayout = tabContentLayout(scroll);

    m_autohideCheck = new QCheckBox(QStringLiteral("Auto-hide toolbar"));
    m_autohideCheck->setToolTip(QStringLiteral("Show toolbar only when mouse is over the note"));
    tabLayout->addWidget(m_autohideCheck);
    connect(m_autohideCheck, &QCheckBox::toggled, this, &SettingsDialog::emitChanged);

    auto *timeoutRow = new QHBoxLayout;
    timeoutRow->addSpacing(24);
    timeoutRow->addWidget(new QLabel(QStringLiteral("Hide delay:")));
    m_timeoutSpin = new QSpinBox;
    m_timeoutSpin->setRange(100, 10000);
    m_timeoutSpin->setSingleStep(100);
    m_timeoutSpin->setSuffix(QStringLiteral(" ms"));
    m_timeoutSpin->setToolTip(QStringLiteral("Delay before hiding toolbar after mouse leaves"));
    SettingsRowUtils::configureSpinbox(m_timeoutSpin);
    timeoutRow->addWidget(m_timeoutSpin);
    SettingsRowUtils::addStepButtons(m_timeoutSpin, this, timeoutRow);
    timeoutRow->addStretch();
    tabLayout->addLayout(timeoutRow);
    connect(m_timeoutSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, &SettingsDialog::emitChanged);

    auto updateTimeoutEnabled = [this, timeoutRow](bool enabled) {
        m_timeoutSpin->setEnabled(enabled);
        for (int i = 0; i < timeoutRow->count(); ++i) {
            if (auto *btn = qobject_cast<QPushButton *>(timeoutRow->itemAt(i)->widget()))
                btn->setEnabled(enabled);
        }
    };
    connect(m_autohideCheck, &QCheckBox::toggled, this, updateTimeoutEnabled);
    updateTimeoutEnabled(m_autohideCheck->isChecked());

    tabLayout->addSpacing(8);
    auto *layoutHeader = new QHBoxLayout;
    layoutHeader->addWidget(new QLabel(QStringLiteral("Toolbar layout:")));
    layoutHeader->addStretch();
    tabLayout->addLayout(layoutHeader);

    m_toolbarLayoutList = new QListWidget;
    m_toolbarLayoutList->setDragDropMode(QAbstractItemView::InternalMove);
    m_toolbarLayoutList->setSelectionMode(QAbstractItemView::SingleSelection);
    m_toolbarLayoutList->setMinimumHeight(150);
    m_toolbarLayoutList->setStyleSheet(
        QStringLiteral("QListWidget { background: %1; color: %2; border: 1px solid %3; border-radius: 3px; }"
                       "QListWidget::item { padding: 4px 8px; color: %2; }"
                       "QListWidget::item:selected { background: %4; color: %2; }"
                       "QListWidget::item:hover { background: %5; }"
                       "QListView::indicator { width: 14px; height: 14px; border: 1px solid %6;"
                       "  border-radius: 2px; background: %7; }"
                       "QListView::indicator:checked { background: %8; border-color: %8;"
                       "  color: white; image: url(:/icons/checked.svg); }"
                       "QListView::indicator:hover { border-color: %8; }")
            .arg(Design::Panel,
                 Design::Foreground,
                 Design::Border,
                 Design::Selected,
                 Design::Hover,
                 Design::Separator,
                 Design::Background,
                 Design::Accent));
    tabLayout->addWidget(m_toolbarLayoutList, 1);

    auto *layoutBtnRow = new QHBoxLayout;
    auto *addSepBtn    = new QPushButton(QStringLiteral("+ Separator"));
    addSepBtn->setToolTip(QStringLiteral("Insert a visual separator at the current position"));
    addSepBtn->setFixedWidth(100);
    addSepBtn->setStyleSheet(Design::buttonStyle("4px 8px", Design::Accent));
    connect(addSepBtn, &QPushButton::clicked, this, [this]() {
        auto *item       = makeSeparatorListItem(QStringLiteral("|"), QStringLiteral("\u2500\u2500\u2500"));
        int   currentRow = m_toolbarLayoutList->currentRow();
        if (currentRow >= 0)
            m_toolbarLayoutList->insertItem(currentRow + 1, item);
        else
            m_toolbarLayoutList->addItem(item);
        notifyToolbarLayoutChanged();
    });
    layoutBtnRow->addWidget(addSepBtn);

    auto *removeBtn = new QPushButton(QStringLiteral("Remove"));
    removeBtn->setToolTip(QStringLiteral("Remove the selected item"));
    removeBtn->setFixedWidth(100);
    removeBtn->setStyleSheet(Design::buttonStyle("4px 8px", Design::Danger));
    connect(removeBtn, &QPushButton::clicked, this, [this]() {
        int row = m_toolbarLayoutList->currentRow();
        if (row >= 0) {
            auto   *item = m_toolbarLayoutList->item(row);
            QString id   = item->data(Qt::UserRole).toString();
            if (id != QStringLiteral("|"))
                return;
            item = m_toolbarLayoutList->takeItem(row);
            delete item;
            notifyToolbarLayoutChanged();
        }
    });
    layoutBtnRow->addWidget(removeBtn);

    layoutBtnRow->addStretch();

    auto *resetBtn = new QPushButton(QStringLiteral("Reset to default"));
    resetBtn->setFixedWidth(120);
    resetBtn->setStyleSheet(Design::buttonStyle("4px 8px", Design::Danger));
    connect(resetBtn, &QPushButton::clicked, this, [this]() {
        m_toolbarLayout    = Settings::defaultToolbarLayout();
        m_toolbarLayoutVis = Settings::defaultToolbarLayoutVisibility();
        buildToolbarLayoutList();
        emitChanged();
    });
    layoutBtnRow->addWidget(resetBtn);

    tabLayout->addLayout(layoutBtnRow);

    buildToolbarLayoutList();

    connect(m_toolbarLayoutList, &QListWidget::itemChanged, this, [this](QListWidgetItem *item) {
        Q_UNUSED(item);
        notifyToolbarLayoutChanged();
    });
    connect(
        m_toolbarLayoutList->model(), &QAbstractItemModel::rowsMoved, this, [this]() { notifyToolbarLayoutChanged(); });

    tabLayout->addStretch();
    m_tabWidget->addTab(scroll, QStringLiteral("Toolbar"));
}

void SettingsDialog::setupStartupTab()
{
    auto *scroll = makeTabScroll(m_tabWidget);
    auto *layout = tabContentLayout(scroll);

    m_autostartCheck = new QCheckBox(QStringLiteral("Open a new note on startup"));
    layout->addWidget(m_autostartCheck);
    connect(m_autostartCheck, &QCheckBox::toggled, this, &SettingsDialog::emitChanged);

    m_startHiddenCheck = new QCheckBox(QStringLiteral("Start with notes hidden"));
    layout->addWidget(m_startHiddenCheck);
    connect(m_startHiddenCheck, &QCheckBox::toggled, this, &SettingsDialog::emitChanged);

    m_defaultOnTopCheck = new QCheckBox(QStringLiteral("New notes always on top"));
    layout->addWidget(m_defaultOnTopCheck);
    connect(m_defaultOnTopCheck, &QCheckBox::toggled, this, &SettingsDialog::emitChanged);

    m_forceXcbCheck = new QCheckBox(QStringLiteral("Force X11 backend on Wayland (restart required)"));
    m_forceXcbCheck->setToolTip(QStringLiteral("Check: forces X11 via XWayland for on-top & opacity support.\n"
                                               "Uncheck: returns to native Wayland backend."));
    layout->addWidget(m_forceXcbCheck);
    connect(m_forceXcbCheck, &QCheckBox::toggled, this, &SettingsDialog::emitChanged);

    layout->addStretch();
    m_tabWidget->addTab(scroll, QStringLiteral("Startup"));
}

void SettingsDialog::setupShortcutsTab()
{
    m_keybindingsPage
        = new KeybindingsPage(m_viewModel->keybindingConfiguration(), m_viewModel->mutableLocalKeybindings());
    m_tabWidget->addTab(m_keybindingsPage, QStringLiteral("Shortcuts"));
    connect(m_keybindingsPage, &KeybindingsPage::changed, this, &SettingsDialog::emitChanged);
}

Settings SettingsDialog::settings() const
{
    Settings result;
    result.setBgColor(m_selectedColor);
    result.setTextColor(m_selectedFontColor);
    result.setFontFamily(m_fontFamilyRow->currentFont().family());
    result.setFontSize(m_fontSizeRow->value());
    result.setAutostartNewNote(m_autostartCheck->isChecked());
    result.setStartHidden(m_startHiddenCheck->isChecked());
    result.setDefaultOnTop(m_defaultOnTopCheck->isChecked());
    result.setForceXcbOnWayland(m_forceXcbCheck->isChecked());
    result.setAutohideToolbar(m_autohideCheck->isChecked());
    result.setAutohideTimeout(m_timeoutSpin->value());
    result.setIconSize(m_iconSizeRow->value());
    result.setOpacity(m_selectedOpacity);
    result.setPinHoverOpacity(m_pinHoverOpacityRow->value());
    result.setPinIdleOpacity(m_pinIdleOpacityRow->value());
    result.setCustomColors(m_customColors);
    result.setNotesDir(m_viewModel->settings().notesDir());
    {
        QHash<QString, QKeySequence> kb;
        for (auto it = m_viewModel->localKeybindings().constBegin(); it != m_viewModel->localKeybindings().constEnd();
             ++it)
            kb[it.key().toString()] = it.value();
        result.setKeybindings(kb);
    }
    result.setToolbarLayout(m_toolbarLayout);
    result.setToolbarLayoutVisibility(m_toolbarLayoutVis);
    return result;
}

void SettingsDialog::buildToolbarLayoutList()
{
    const QSignalBlocker blocker(m_toolbarLayoutList);
    m_toolbarLayoutList->clear();

    const QStringList   &layout = m_toolbarLayout;
    const QSet<QString> &vis    = m_toolbarLayoutVis;

    for (const auto &item : layout) {
        if (item == QStringLiteral("|")) {
            m_toolbarLayoutList->addItem(
                makeSeparatorListItem(QStringLiteral("|"), QStringLiteral("\u2500\u2500\u2500")));
        }
        else if (item == QStringLiteral(">>")) {
            auto *sep = makeSeparatorListItem(QStringLiteral(">>"),
                                              QStringLiteral("\u2500\u2500\u2500 right-aligned \u2500\u2500\u2500"));
            sep->setFlags(sep->flags() & ~Qt::ItemIsDragEnabled & ~Qt::ItemIsSelectable & ~Qt::ItemIsEditable);
            m_toolbarLayoutList->addItem(sep);
        }
        else {
            auto *def = findActionDef(item);

            QString label   = def ? def->label : item;
            bool    visible = vis.contains(item);

            auto *li = new QListWidgetItem;
            li->setData(Qt::UserRole, item);
            li->setCheckState(visible ? Qt::Checked : Qt::Unchecked);

            if (def) {
                li->setIcon(Icons::icon(def->icon, 16));
                li->setText(label);
            }
            else {
                li->setText(label);
            }
            m_toolbarLayoutList->addItem(li);
        }
    }
}

void SettingsDialog::saveToolbarLayout()
{
    m_toolbarLayout.clear();
    m_toolbarLayoutVis.clear();

    for (int i = 0; i < m_toolbarLayoutList->count(); ++i) {
        auto   *li = m_toolbarLayoutList->item(i);
        QString id = li->data(Qt::UserRole).toString();
        if (id == QStringLiteral(">>")) {
            m_toolbarLayout.append(id);
            continue;
        }
        m_toolbarLayout.append(id);
        if (id != QStringLiteral("|") && li->checkState() == Qt::Checked)
            m_toolbarLayoutVis.insert(id);
    }
}

void SettingsDialog::notifyToolbarLayoutChanged()
{
    saveToolbarLayout();
    emitChanged();
}
