#pragma once

#include <QObject>
#include "Settings.h"

class QWidget;

class INoteController : public QObject
{
    Q_OBJECT
public:
    using QObject::QObject;
    ~INoteController() override = default;

    virtual void     saveNow()                                                           = 0;
    virtual void     showWindow()                                                        = 0;
    virtual void     hideWindow()                                                        = 0;
    virtual bool     isWindowVisible() const                                             = 0;
    virtual QWidget *noteWidget() const                                                  = 0;
    virtual void     applyGlobalSettings(const Settings &settings, bool restoreAutohide) = 0;
    virtual void     setAutohide(bool on)                                                = 0;
    virtual bool     isPinned() const                                                    = 0;
    virtual void     setPinSoloMode(bool on)                                             = 0;
    virtual void     setGhostMode(bool on)                                               = 0;
    virtual bool     isGhostMode() const                                                 = 0;

signals:
    void newNoteRequested();
    void preferencesRequested();
    void noteDeleted(INoteController *controller);
    void hideAllNotesRequested();
};
