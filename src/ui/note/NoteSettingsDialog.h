#pragma once

#include "BaseDialog.h"
#include "ColorRow.h"
#include "SpinRow.h"
#include "SliderSpinRow.h"
#include "FontFamilyRow.h"

class NoteSettingsDialog : public BaseDialog
{
    Q_OBJECT
public:
    explicit NoteSettingsDialog(QWidget *parent = nullptr);

    ColorRow *fontColorRow() const
    {
        return m_fontColorRow;
    }
    ColorRow *bgColorRow() const
    {
        return m_bgColorRow;
    }
    SpinRow *fontSizeRow() const
    {
        return m_fontSizeRow;
    }
    FontFamilyRow *fontFamilyRow() const
    {
        return m_fontFamilyRow;
    }
    SliderSpinRow *opacityRow() const
    {
        return m_opacityRow;
    }
    SpinRow *iconSizeRow() const
    {
        return m_iconSizeRow;
    }

private:
    void setupUI();

    ColorRow      *m_fontColorRow{nullptr};
    ColorRow      *m_bgColorRow{nullptr};
    SpinRow       *m_fontSizeRow{nullptr};
    FontFamilyRow *m_fontFamilyRow{nullptr};
    SliderSpinRow *m_opacityRow{nullptr};
    SpinRow       *m_iconSizeRow{nullptr};
};
