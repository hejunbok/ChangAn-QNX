#include "Fonts.h"

#include <QDir>
#include <QFontDatabase>
#include <QString>
#include <QStringList>

#include <iostream>

static bool loadFonts(const QFileInfoList &fontFiles)
{
    Q_FOREACH(const QFileInfo &ffn, fontFiles) {
        const int id = QFontDatabase::addApplicationFont(ffn.absoluteFilePath());
        if (id == -1) {
            std::cerr << "Could not add font from file " << qPrintable(ffn.absoluteFilePath()) << std::endl;
            return false;
        }
    }

    return true;
}

bool QnxCarUi::loadFonts(const QString &basedir)
{
    QStringList fontPaths;
#ifndef _WIN32
    fontPaths << basedir;
#else
    Q_UNUSED(basedir)
#endif

#ifdef Q_OS_QNX
    fontPaths << QLatin1String("/base/usr/fonts/font_repository/monotype/");
#endif

    Q_FOREACH(const QString &fontPath, fontPaths) {
        if (!loadFonts(QDir(fontPath).entryInfoList(QStringList() << QStringLiteral("*.ttf"))))
            return false;
    }

    return true;
}
