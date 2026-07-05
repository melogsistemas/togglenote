#include "KeybindingService.h"
#include "ISettingsProvider.h"
#include <QMetaObject>

KeybindingService::KeybindingService(ISettingsProvider *provider, QObject *parent)
    : QObject(parent)
    , m_provider(provider)
{
    for (const ActionDefinition &def : allActionDefinitions())
        m_defaults.insert(def.id, def);
    reload();
}

void KeybindingService::reload()
{
    rebuildMap();
    emit bindingsChanged();
}

void KeybindingService::rebuildMap()
{
    m_effective.clear();
    const Settings &settings = m_provider->settings();
    const auto     &kb       = settings.keybindings();

    for (auto it = m_defaults.constBegin(); it != m_defaults.constEnd(); ++it) {
        QString key = it.key().toString();
        if (kb.contains(key))
            m_effective.insert(it.key(), kb.value(key));
        else
            m_effective.insert(it.key(), it.value().defaultShortcut);
    }
}

QKeySequence KeybindingService::shortcut(ActionId actionId) const
{
    auto it = m_effective.find(actionId);
    if (it != m_effective.end())
        return it.value();
    auto def = m_defaults.find(actionId);
    if (def != m_defaults.end())
        return def.value().defaultShortcut;
    return {};
}

QMetaObject::Connection KeybindingService::onBindingsChanged(QObject *receiver, std::function<void()> callback)
{
    return connect(this, &KeybindingService::bindingsChanged, receiver, std::move(callback));
}

ActionDefinition KeybindingService::actionDef(ActionId actionId) const
{
    auto it = m_defaults.find(actionId);
    if (it != m_defaults.end())
        return it.value();
    return {};
}

QList<ActionDefinition> KeybindingService::allActions() const
{
    QList<ActionDefinition> result;
    for (auto it = m_defaults.constBegin(); it != m_defaults.constEnd(); ++it)
        result.append(it.value());
    return result;
}

void KeybindingService::setValidator(Validator validator)
{
    m_validator = std::move(validator);
}

QString KeybindingService::validateShortcut(ActionId actionId, const QKeySequence &seq) const
{
    if (!m_defaults.contains(actionId))
        return QStringLiteral("Unknown action.");

    if (m_validator && !seq.isEmpty()) {
        QString err = m_validator(actionId, seq);
        if (!err.isEmpty())
            return err;
    }

    if (!seq.isEmpty()) {
        for (auto it = m_effective.constBegin(); it != m_effective.constEnd(); ++it) {
            if (it.key() != actionId && it.value() == seq) {
                const ActionDefinition &def = m_defaults.value(it.key());
                return QStringLiteral("\"%1\" is already assigned to \"%2\"")
                    .arg(seq.toString(QKeySequence::NativeText), def.label);
            }
        }
    }

    return {};
}

QString KeybindingService::setShortcut(ActionId actionId, const QKeySequence &seq)
{
    QString err = validateShortcut(actionId, seq);
    if (!err.isEmpty())
        return err;

    Settings settings = m_provider->settings();
    auto     kb       = settings.keybindings();
    QString  key      = actionId.toString();
    if (seq == m_defaults.value(actionId).defaultShortcut)
        kb.remove(key);
    else
        kb[key] = seq;
    settings.setKeybindings(kb);

    m_provider->save(settings);
    rebuildMap();
    emit bindingsChanged();
    return {};
}

void KeybindingService::resetShortcut(ActionId actionId)
{
    if (!m_defaults.contains(actionId))
        return;

    Settings settings = m_provider->settings();
    auto     kb       = settings.keybindings();
    kb.remove(actionId.toString());
    settings.setKeybindings(kb);
    m_provider->save(settings);
    rebuildMap();
    emit bindingsChanged();
}

void KeybindingService::resetAllShortcuts()
{
    Settings settings = m_provider->settings();
    auto     kb       = settings.keybindings();
    kb.clear();
    settings.setKeybindings(kb);
    m_provider->save(settings);
    rebuildMap();
    emit bindingsChanged();
}
