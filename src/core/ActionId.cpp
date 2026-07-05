#include "ActionId.h"
#include "ActionDefinition.h"

QString ActionId::toString() const
{
    static const QHash<ActionId, QString> map = [] {
        QHash<ActionId, QString> m;
        for (const auto &def : allActionDefinitions())
            m.insert(def.id, def.key);
        return m;
    }();
    return map.value(*this);
}

ActionId ActionId::fromString(const QString &str)
{
    static const QHash<QString, ActionId> map = [] {
        QHash<QString, ActionId> m;
        for (const auto &def : allActionDefinitions())
            m.insert(def.key, def.id);
        return m;
    }();
    auto it = map.constFind(str);
    if (it != map.constEnd())
        return *it;
    return ActionId{-1};
}
