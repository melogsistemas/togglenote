#include "SettingsRepository.h"

#include "ActionDefinition.h"
#include "ActionId.h"

#include <QDir>
#include <QFileInfo>
#include <QSettings>


namespace
{
const QString SETTINGS_FILE      = QStringLiteral("settings.conf");
const QString KEY_AUTOSTART      = QStringLiteral("autostartNewNote");
const QString KEY_START_HIDDEN   = QStringLiteral("startHidden");
const QString KEY_DEFAULT_ON_TOP = QStringLiteral("defaultOnTop");
const QString KEY_AUTOHIDE_TOOL  = QStringLiteral("autohideToolbar");
const QString KEY_AUTOHIDE_TO    = QStringLiteral("autohideTimeout");
const QString KEY_BG_COLOR       = QStringLiteral("bgColor");
const QString KEY_TEXT_COLOR     = QStringLiteral("textColor");
const QString KEY_FONT_FAMILY    = QStringLiteral("fontFamily");
const QString KEY_FONT_SIZE      = QStringLiteral("fontSize");
const QString KEY_OPACITY        = QStringLiteral("opacity");
const QString KEY_PIN_HOVER_OP   = QStringLiteral("pinHoverOpacity");
const QString KEY_PIN_IDLE_OP    = QStringLiteral("pinIdleOpacity");
const QString KEY_ICON_SIZE      = QStringLiteral("iconSize");
const QString KEY_CUSTOM_CLRS    = QStringLiteral("customColors");
const QString KEY_VISIBLE_BTNS   = QStringLiteral("visibleButtons");
const QString KEY_FORCE_XCB      = QStringLiteral("forceXcbOnWayland");
const QString KEY_NOTES_DIR      = QStringLiteral("notesDir");
const QString KEY_TB_LAYOUT      = QStringLiteral("toolbarLayout");
const QString KEY_TB_VIS         = QStringLiteral("toolbarLayoutVisibility");
const QString GROUP_GENERAL      = QStringLiteral("General");
const QString GROUP_KEYBINDINGS  = QStringLiteral("Keybindings");
} // namespace

SettingsRepository::SettingsRepository(const QString &configDir)
    : m_configDir(configDir)
{}

Settings SettingsRepository::getSettings()
{
    QString settingsPath = m_configDir + QStringLiteral("/") + SETTINGS_FILE;
    QDir().mkpath(QFileInfo(settingsPath).absolutePath());
    QSettings qSettings(settingsPath, QSettings::IniFormat);

    Settings settings;
    qSettings.beginGroup(GROUP_GENERAL);
    if (qSettings.contains(KEY_AUTOSTART))
        settings.setAutostartNewNote(qSettings.value(KEY_AUTOSTART).toBool());
    if (qSettings.contains(KEY_START_HIDDEN))
        settings.setStartHidden(qSettings.value(KEY_START_HIDDEN).toBool());
    if (qSettings.contains(KEY_DEFAULT_ON_TOP))
        settings.setDefaultOnTop(qSettings.value(KEY_DEFAULT_ON_TOP).toBool());
    if (qSettings.contains(KEY_AUTOHIDE_TOOL))
        settings.setAutohideToolbar(qSettings.value(KEY_AUTOHIDE_TOOL).toBool());
    if (qSettings.contains(KEY_AUTOHIDE_TO))
        settings.setAutohideTimeout(qSettings.value(KEY_AUTOHIDE_TO).toInt());
    if (qSettings.contains(KEY_FONT_FAMILY))
        settings.setFontFamily(qSettings.value(KEY_FONT_FAMILY).toString());
    if (qSettings.contains(KEY_FONT_SIZE))
        settings.setFontSize(qSettings.value(KEY_FONT_SIZE).toInt());
    if (qSettings.contains(KEY_OPACITY))
        settings.setOpacity(qSettings.value(KEY_OPACITY).toInt());
    if (qSettings.contains(KEY_PIN_HOVER_OP))
        settings.setPinHoverOpacity(qSettings.value(KEY_PIN_HOVER_OP).toInt());
    if (qSettings.contains(KEY_PIN_IDLE_OP))
        settings.setPinIdleOpacity(qSettings.value(KEY_PIN_IDLE_OP).toInt());
    if (qSettings.contains(KEY_BG_COLOR))
        settings.setBgColor(QColor(qSettings.value(KEY_BG_COLOR).toString()));
    if (qSettings.contains(KEY_TEXT_COLOR))
        settings.setTextColor(QColor(qSettings.value(KEY_TEXT_COLOR).toString()));
    if (qSettings.contains(KEY_ICON_SIZE))
        settings.setIconSize(qSettings.value(KEY_ICON_SIZE).toInt());
    if (qSettings.contains(KEY_CUSTOM_CLRS))
        settings.setCustomColors(qSettings.value(KEY_CUSTOM_CLRS).toStringList());

    if (qSettings.contains(KEY_VISIBLE_BTNS)) {
        QStringList buttonIds = qSettings.value(KEY_VISIBLE_BTNS).toStringList();
        if (!buttonIds.isEmpty()) {
            QList<ButtonId> buttons;
            buttons.reserve(buttonIds.size());
            for (const QString &buttonId : buttonIds)
                buttons.append(static_cast<ButtonId>(buttonId.toInt()));
            settings.setVisibleButtons(buttons);
        }
    }

    if (qSettings.contains(KEY_TB_LAYOUT))
        settings.setToolbarLayout(qSettings.value(KEY_TB_LAYOUT).toStringList());
    else
        settings.setToolbarLayout(Settings::defaultToolbarLayout());

    if (qSettings.contains(KEY_TB_VIS)) {
        QStringList visList = qSettings.value(KEY_TB_VIS).toStringList();
        settings.setToolbarLayoutVisibility(QSet<QString>(visList.begin(), visList.end()));
    }
    else {
        settings.setToolbarLayoutVisibility(Settings::defaultToolbarLayoutVisibility());
    }

    if (migrateToolbarLayout(settings))
        save(settings);
    if (qSettings.contains(KEY_NOTES_DIR))
        settings.setNotesDir(qSettings.value(KEY_NOTES_DIR).toString());
    if (qSettings.contains(KEY_FORCE_XCB))
        settings.setForceXcbOnWayland(qSettings.value(KEY_FORCE_XCB).toBool());
    qSettings.endGroup();

    qSettings.beginGroup(GROUP_KEYBINDINGS);
    {
        QHash<QString, QKeySequence> kb;
        for (const QString &key : qSettings.allKeys())
            kb.insert(key, QKeySequence(qSettings.value(key).toString()));
        settings.setKeybindings(kb);
    }
    qSettings.endGroup();

    return settings;
}

bool SettingsRepository::migrateToolbarLayout(Settings &settings)
{
    QStringList   layout  = settings.toolbarLayout();
    QSet<QString> vis     = settings.toolbarLayoutVisibility();
    bool          changed = false;

    // Remove obsolete entries (ButtonId::None items that produce no button)
    QStringList cleaned;
    for (const auto &item : layout) {
        if (item == QStringLiteral("|") || item == QStringLiteral(">>")) {
            cleaned.append(item);
            continue;
        }
        ActionId id = ActionId::fromString(item);
        if (id.value == -1) {
            changed = true;
            continue;
        }
        bool obsolete = false;
        for (const auto &def : allActionDefinitions()) {
            if (def.id == id) {
                if (def.btn == ButtonId::None)
                    obsolete = true;
                break;
            }
        }
        if (!obsolete)
            cleaned.append(item);
        else
            changed = true;
    }

    // Remove duplicates
    QStringList   deduped;
    QSet<QString> seen;
    for (const auto &item : cleaned) {
        if (item == QStringLiteral("|") || item == QStringLiteral(">>")) {
            deduped.append(item);
            continue;
        }
        if (seen.contains(item)) {
            changed = true;
            continue;
        }
        seen.insert(item);
        deduped.append(item);
    }
    layout = deduped;

    // Append missing default items
    for (const auto &id : Settings::defaultToolbarLayout()) {
        if (!layout.contains(id)) {
            layout.append(id);
            if (id != QStringLiteral("|") && id != QStringLiteral(">>"))
                vis.insert(id);
            changed = true;
        }
    }

    // Clean obsolete entries from visibility set
    QSet<QString> validVis;
    for (const auto &item : layout) {
        if (item != QStringLiteral("|") && item != QStringLiteral(">>"))
            validVis.insert(item);
    }
    QSet<QString> oldVis = vis;
    vis.intersect(validVis);
    if (vis != oldVis)
        changed = true;

    if (changed) {
        settings.setToolbarLayout(layout);
        settings.setToolbarLayoutVisibility(vis);
    }
    return changed;
}

bool SettingsRepository::save(const Settings &settings)
{
    QString settingsPath = m_configDir + QStringLiteral("/") + SETTINGS_FILE;
    QDir().mkpath(QFileInfo(settingsPath).absolutePath());
    QSettings qSettings(settingsPath, QSettings::IniFormat);

    qSettings.beginGroup(GROUP_GENERAL);
    qSettings.setValue(KEY_AUTOSTART, settings.autostartNewNote());
    qSettings.setValue(KEY_START_HIDDEN, settings.startHidden());
    qSettings.setValue(KEY_DEFAULT_ON_TOP, settings.defaultOnTop());
    qSettings.setValue(KEY_AUTOHIDE_TOOL, settings.autohideToolbar());
    qSettings.setValue(KEY_AUTOHIDE_TO, settings.autohideTimeout());
    qSettings.setValue(KEY_BG_COLOR, settings.bgColor().name());
    qSettings.setValue(KEY_TEXT_COLOR, settings.textColor().name());
    qSettings.setValue(KEY_FONT_FAMILY, settings.fontFamily());
    qSettings.setValue(KEY_FONT_SIZE, settings.fontSize());
    qSettings.setValue(KEY_OPACITY, settings.opacity());
    qSettings.setValue(KEY_PIN_HOVER_OP, settings.pinHoverOpacity());
    qSettings.setValue(KEY_PIN_IDLE_OP, settings.pinIdleOpacity());
    qSettings.setValue(KEY_ICON_SIZE, settings.iconSize());
    qSettings.setValue(KEY_CUSTOM_CLRS, settings.customColors());

    {
        QStringList buttonIds;
        buttonIds.reserve(settings.visibleButtons().size());
        for (ButtonId button : settings.visibleButtons())
            buttonIds.append(QString::number(static_cast<int>(button)));
        qSettings.setValue(KEY_VISIBLE_BTNS, buttonIds);
    }

    qSettings.setValue(KEY_TB_LAYOUT, settings.toolbarLayout());
    qSettings.setValue(
        KEY_TB_VIS, QStringList(settings.toolbarLayoutVisibility().begin(), settings.toolbarLayoutVisibility().end()));
    qSettings.setValue(KEY_NOTES_DIR, settings.notesDir());
    qSettings.setValue(KEY_FORCE_XCB, settings.forceXcbOnWayland());
    qSettings.endGroup();

    qSettings.beginGroup(GROUP_KEYBINDINGS);
    qSettings.remove(QString());
    for (auto it = settings.keybindings().constBegin(); it != settings.keybindings().constEnd(); ++it)
        qSettings.setValue(it.key(), it.value().toString());
    qSettings.endGroup();

    return true;
}
