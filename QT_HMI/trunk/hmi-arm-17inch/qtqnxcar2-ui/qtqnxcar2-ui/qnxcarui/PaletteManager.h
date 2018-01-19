#ifndef PALETTEMANAGER_H
#define PALETTEMANAGER_H

#include "qtqnxcar_ui_export.h"

#include <QObject>

namespace QnxCarUi {
    class PaletteData;
    class PaletteItem;
    class ThemeManager;

class QTQNXCAR_UI_EXPORT PaletteManager : public QObject
{
    Q_OBJECT

    Q_PROPERTY(PaletteItem* palette READ palette CONSTANT)

public:
    explicit PaletteManager(ThemeManager *theme, QObject *parent = 0);

    PaletteItem *palette() const;

private Q_SLOTS:
    void updatePalette();

private:
    PaletteData paletteForTheme(const QString& theme) const;

    PaletteItem *m_paletteItem;
    ThemeManager *m_themeManager;
};

}

#endif
