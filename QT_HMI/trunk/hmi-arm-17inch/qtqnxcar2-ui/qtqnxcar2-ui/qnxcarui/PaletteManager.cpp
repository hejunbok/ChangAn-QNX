#include "PaletteManager.h"

#include "Palette.h"
#include "ThemeManager.h"

#include "qnxcar/Theme.h"

#include <QFile>
#include <QQmlEngine>
#include <QQmlComponent>
#include <iostream>

namespace QnxCarUi {

PaletteManager::PaletteManager(ThemeManager *theme, QObject *parent)
    : QObject(parent)
    , m_paletteItem(new PaletteItem(this))
    , m_themeManager(theme)
{
    connect(m_themeManager->theme(), SIGNAL(themeChanged(QString)), this, SLOT(updatePalette()));

    // initialize
    updatePalette();
}

PaletteItem* PaletteManager::palette() const
{
    return m_paletteItem;
}

PaletteData PaletteManager::paletteForTheme(const QString& theme) const
{
    QString path = QString(QStringLiteral("%1/palettes/%2.qml"))
        .arg(m_themeManager->assetsDirectory())
        .arg(theme);

    if (!QFile(path).exists()) {
        qWarning() << "Palette definition not found in:" << path;
        return PaletteData();
    }

    QQmlEngine engine;
    QQmlComponent component(&engine, path);
    QObject *object = component.create();
    if (!object) {
        return PaletteData();
    }

    QVariant paletteProp = object->property("palette");
    PaletteParser parser(paletteProp);
    return parser.parse();
}

void PaletteManager::updatePalette()
{
    const QString theme = m_themeManager->theme()->current();
    PaletteData p = paletteForTheme(theme);
    m_paletteItem->setPalette(p);
}

}
