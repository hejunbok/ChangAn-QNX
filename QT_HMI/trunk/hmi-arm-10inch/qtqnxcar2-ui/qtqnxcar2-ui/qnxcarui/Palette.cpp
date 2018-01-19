#include "Palette.h"

#include <QMetaEnum>
#include <QMetaObject>

namespace QnxCarUi {

QColor PaletteData::color(int role) const
{
    return m_colorMap.value(role);
}

void PaletteData::setColor(int role, const QColor& color)
{
    m_colorMap[role] = color;
}

bool PaletteData::unsetColor(int role)
{
    return m_colorMap.remove(role);
}

bool PaletteData::operator==(const PaletteData &other) const
{
    return m_colorMap == other.m_colorMap;
}

bool PaletteData::operator!=(const PaletteData& other) const
{
    return m_colorMap != other.m_colorMap;
}

bool PaletteData::isEmpty() const
{
    return m_colorMap.isEmpty();
}

QDebug operator<<(QDebug dbg, const PaletteData& o)
{
    return dbg << o.m_colorMap;
}

PaletteParser::PaletteParser(const QVariant &data, QObject *parent)
    : QObject(parent)
    , m_data(data)
{
}

PaletteData PaletteParser::parse() const
{
    PaletteData p;
    QVariantMap map = m_data.toMap();
    QVariantMap::const_iterator it = map.constBegin();
    while (it != map.constEnd()) {
        PaletteItem::ColorRole role = PaletteItem::fromString(it.key());
        if (role == PaletteItem::NoRole || !it.value().canConvert<QColor>()) {
            ++it;
            continue;
        }

        p.setColor(role, it.value().value<QColor>());
        ++it;
    }
    return p;
}

PaletteItem::PaletteItem(QObject* parent)
    : QObject(parent)
{
    qRegisterMetaType<PaletteData>();
}

QColor PaletteItem::color(const PaletteItem::ColorRole& role) const
{
    return m_palette.color(role);
}

PaletteItem::ColorRole PaletteItem::fromString(const QString& str)
{
    const int index = staticMetaObject.indexOfEnumerator("ColorRole");
    Q_ASSERT(index != -1);
    const QMetaEnum metaEnum = staticMetaObject.enumerator(index);
    const int value = metaEnum.keysToValue(qPrintable(str));
    if (value == -1)
        return NoRole;

    return static_cast<ColorRole>(value);
}

}
