#pragma once

#include <QObject>
#include <QHash>
#include <QKeySequence>
#include <functional>
#include "IKeybindingProvider.h"
#include "IKeybindingConfiguration.h"
#include "ActionId.h"
#include "ActionDefinition.h"

class ISettingsProvider;

class KeybindingService : public QObject, public IKeybindingConfiguration
{
    Q_OBJECT
public:
    using Validator = std::function<QString(ActionId actionId, const QKeySequence &seq)>;

    explicit KeybindingService(ISettingsProvider *provider, QObject *parent = nullptr);

    QKeySequence            shortcut(ActionId actionId) const override;
    QMetaObject::Connection onBindingsChanged(QObject *receiver, std::function<void()> callback) override;

    ActionDefinition        actionDef(ActionId actionId) const;
    QList<ActionDefinition> allActions() const;

    void    setValidator(Validator validator);
    QString validateShortcut(ActionId actionId, const QKeySequence &seq) const override;
    QString setShortcut(ActionId actionId, const QKeySequence &seq);
    void    resetShortcut(ActionId actionId);
    void    resetAllShortcuts();
    void    reload();

signals:
    void bindingsChanged();

private:
    void rebuildMap();

    ISettingsProvider                *m_provider;
    Validator                         m_validator;
    QHash<ActionId, ActionDefinition> m_defaults;
    QHash<ActionId, QKeySequence>     m_effective;
};
