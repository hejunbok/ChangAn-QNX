#include "StandardDirs.h"

#include <QCoreApplication>
#include <QFile>
#include <QString>

namespace QnxCar {

static QString ensureTrailingSeparator(const QString &path)
{
    if (path.endsWith(QLatin1Char('/')) || path.endsWith(QLatin1Char('\\')))
        return path;
    else
        return path + QLatin1Char('/');
}

static QString sharedDirectory()
{
    return QCoreApplication::applicationDirPath() + QStringLiteral("/../share/qnxcar2");
}

QString StandardDirs::assetsDirectory()
{
    static QString assetsPath;
    if (!assetsPath.isEmpty())
        return assetsPath;

    const QByteArray envVar = qgetenv("QNXCAR2_ASSETS_DIR");
    const QString path = !envVar.isEmpty() ? QFile::decodeName(envVar) : sharedDirectory();

    if (QFile::exists(path + QStringLiteral("/qml/main.qml"))) {
        assetsPath = ensureTrailingSeparator(path);
        return assetsPath;
    }

#ifdef ASSETS_PATH
    if (QFile::exists( QStringLiteral(ASSETS_PATH) + QStringLiteral("/qml/main.qml"))) {
        assetsPath = ensureTrailingSeparator(QStringLiteral(ASSETS_PATH));
        return assetsPath;
    }
#endif
    return QString();
}

}
