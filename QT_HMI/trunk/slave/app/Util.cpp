#include "Util.h"

#include <QDebug>
#include <QFileInfo>
#include <QUrl>

Util::Util(QObject *parent)
    : QObject(parent)
{
}

bool Util::fileExists(const QUrl &url) const
{
    if (!url.isValid())
        return false;

    if (url.isLocalFile()) {
        const QFileInfo fi(url.toLocalFile());
        return fi.exists();
    }
    return true;
}
