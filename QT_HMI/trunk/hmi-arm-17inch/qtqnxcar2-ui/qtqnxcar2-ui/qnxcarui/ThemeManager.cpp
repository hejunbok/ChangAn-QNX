#include "ThemeManager.h"

#include <QDebug>
#include <QFile>

using QnxCar::Theme;

namespace QnxCarUi {

ThemeManager::ThemeManager(ResolutionManager::Resolution resolution, Theme* theme, QObject* parent)
    : QObject(parent)
    , m_theme(theme)
    , m_resolution(resolution)
{
}

static QString folderForResolution(ResolutionManager::Resolution r)
{
    switch (r)
    {
    case ResolutionManager::Resolution_800x480:
        return QStringLiteral("800x480");
    case ResolutionManager::Resolution_720p:
        return QStringLiteral("720p");
    case ResolutionManager::Resolution_1920x1200:
        return QStringLiteral("1920x1200");
    case ResolutionManager::Resolution_1280x800:
        return QStringLiteral("1280x800");
    default:
        break;
    }

    Q_ASSERT(!"Unhandled resolution value");
    return QString();
}

QString ThemeManager::pathForTheme(const QString &theme, Theme::Mode mode) const
{
    QString path = m_assetsDirectory;

    path += QStringLiteral("images/themes/");

    path += folderForResolution(m_resolution) + QLatin1Char('/');
    // TODO: Check against available themes?
    path += theme + QLatin1Char('/');

    switch (mode) {
        case Theme::Day:
            //commented out for now since we do not use this structure anymore
            //path += QStringLiteral("day/");
            break;
        case Theme::Night:
            //commented out for now since we do not use this structure anymore
            //path += QStringLiteral("night/");
            break;
        default:
            Q_ASSERT(false);
            break;
    }

    return path;
}

QString ThemeManager::path() const
{
    return pathForTheme(m_theme->current(), m_theme->mode());
}

QUrl ThemeManager::fromTheme(const QString& iconName) const
{
    // check if there is an icon for current-theme/current-mode
    QString icon = path() + iconName;
    if (QFile(icon).exists())
        return QUrl::fromLocalFile(icon);

    // fallback: check if there is an icon for current-theme/day-mode
    icon = pathForTheme(m_theme->current(), Theme::Day) + iconName;
    if (QFile(icon).exists()) {
        return QUrl::fromLocalFile(icon);
    }

    // fallback: check if there is an icon for default-theme/day-mode
    icon = pathForTheme(m_theme->defaultTheme(), Theme::Day) + iconName;
    if (QFile(icon).exists()) {
        return QUrl::fromLocalFile(icon);
    }

    // fallback: return empty string
    qWarning() << "Icon not found:" << iconName;
    return QUrl();
}

QString ThemeManager::assetsDirectory() const
{
    return m_assetsDirectory;
}

void ThemeManager::setAssetsDirectory(const QString &assetsDirectory)
{
    QString path = assetsDirectory;
    if (!path.endsWith(QLatin1Char('/')))
        path.append(QLatin1Char('/'));

    if (m_assetsDirectory == path)
        return;

    m_assetsDirectory = path;
}

Theme* ThemeManager::theme() const
{
    return m_theme;
}

}

