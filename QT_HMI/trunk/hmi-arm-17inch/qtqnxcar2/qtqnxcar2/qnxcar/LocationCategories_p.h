#ifndef LOCATIONCATEGORIES_P_H
#define LOCATIONCATEGORIES_P_H

#include <QString>

namespace QnxCar {

class LocationCategories
{
public:
    static QString typeToIconName(const QString &type)
    {
        if (typeIconMap.isEmpty()) {
            typeIconMap[QLatin1String("accomodation")]   = QStringLiteral("accom");
            typeIconMap[QLatin1String("attraction")]     = QStringLiteral("attract");
            typeIconMap[QLatin1String("hospital")]       = QStringLiteral("hosp");
            typeIconMap[QLatin1String("museum")]         = QStringLiteral("museum");
            typeIconMap[QLatin1String("park")]           = QStringLiteral("park");
            typeIconMap[QLatin1String("parking")]        = QStringLiteral("parking");
            typeIconMap[QLatin1String("public")]         = QStringLiteral("pubserv");
            typeIconMap[QLatin1String("restaurant")]     = QStringLiteral("restaurant");
            typeIconMap[QLatin1String("sports")]         = QStringLiteral("sports");
            typeIconMap[QLatin1String("transportation")] = QStringLiteral("transport");
            typeIconMap[QLatin1String("church")]         = QStringLiteral("church");
        }
        return typeIconMap.value(type);
    }

private:
    static QMap<QString, QString> typeIconMap;
};

}

#endif // LOCATIONCATEGORIES_P_H
