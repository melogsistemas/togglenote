#include "NoteViewModel.h"
#include "INoteService.h"


NoteViewModel::NoteViewModel(const Note &data, INoteService *service, const Settings &globalDefaults, QObject *parent)
    : QObject(parent)
    , m_data(data)
    , m_service(service)
    , m_globalDefaults(globalDefaults)
{
    m_saveTimer = new QTimer(this);
    m_saveTimer->setSingleShot(true);
    constexpr int kAutosaveDelayMs = 500;
    m_saveTimer->setInterval(kAutosaveDelayMs);
    connect(m_saveTimer, &QTimer::timeout, this, &NoteViewModel::save);
}

void NoteViewModel::updateContent(const QString &markdown)
{
    m_data.setContent(markdown);
}

void NoteViewModel::updateGeometry(int width, int height, double xPct, double yPct)
{
    m_data.setWidth(width);
    m_data.setHeight(height);
    m_data.setXPct(xPct);
    m_data.setYPct(yPct);
}

void NoteViewModel::save()
{
    m_service->saveNote(m_data);
    m_data.markSaved();
}

void NoteViewModel::markDirty()
{
    m_data.markDirty();
}

void NoteViewModel::scheduleSave()
{
    m_saveTimer->start();
}

void NoteViewModel::setFontSize(std::optional<int> size)
{
    m_data.setFontSize(size);
    emit styleChanged();
}

void NoteViewModel::setFontFamily(std::optional<QString> family)
{
    m_data.setFontFamily(family);
    emit styleChanged();
}

void NoteViewModel::setTextColor(std::optional<QColor> color)
{
    m_data.setTextColor(color);
    emit styleChanged();
}

void NoteViewModel::setBgColor(std::optional<QColor> color)
{
    m_data.setBgColor(color);
    emit styleChanged();
}

void NoteViewModel::setOpacity(std::optional<int> pct)
{
    m_data.setOpacity(pct);
    emit styleChanged();
}

void NoteViewModel::setIconSize(std::optional<int> sz)
{
    m_data.setIconSize(sz);
    emit styleChanged();
}

void NoteViewModel::setLocked(bool locked)
{
    m_data.setLocked(locked);
}

void NoteViewModel::setPinned(bool pinned)
{
    m_data.setPinned(pinned);
}

void NoteViewModel::setAlwaysOnTop(bool on)
{
    m_data.setAlwaysOnTop(on);
}

QString NoteViewModel::content() const
{
    return m_data.content();
}

bool NoteViewModel::isLocked() const
{
    return m_data.locked();
}

bool NoteViewModel::isPinned() const
{
    return m_data.pinned();
}

bool NoteViewModel::isAlwaysOnTop() const
{
    return m_data.alwaysOnTop();
}

QString NoteViewModel::filename() const
{
    return m_data.filename();
}

int NoteViewModel::width() const
{
    return m_data.width();
}

int NoteViewModel::height() const
{
    return m_data.height();
}

std::optional<int> NoteViewModel::iconSize() const
{
    return m_data.iconSize();
}

QColor NoteViewModel::bgColor() const
{
    return m_data.bgColor().value_or(m_globalDefaults.bgColor());
}

QColor NoteViewModel::textColor() const
{
    return m_data.textColor().value_or(m_globalDefaults.textColor());
}

int NoteViewModel::fontSize() const
{
    return m_data.fontSize().value_or(m_globalDefaults.fontSize());
}

QString NoteViewModel::fontFamily() const
{
    return m_data.fontFamily().value_or(m_globalDefaults.fontFamily());
}

int NoteViewModel::opacity() const
{
    return m_data.opacity().value_or(m_globalDefaults.opacity());
}

void NoteViewModel::setGlobalDefaults(const Settings &s)
{
    m_globalDefaults = s;
}
