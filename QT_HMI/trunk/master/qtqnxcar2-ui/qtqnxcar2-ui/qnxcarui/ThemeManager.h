#ifndef THEMEMANAGER_H
#define THEMEMANAGER_H

#include "qtqnxcar_ui_export.h"

#include "ResolutionManager.h"

#include "qnxcar/Theme.h"

#include <QObject>

class QUrl;

namespace QnxCarUi {

/**
 * @short The class that coordinates theme related tasks
 */
class QTQNXCAR_UI_EXPORT ThemeManager : public QObject
{
    Q_OBJECT

public:
    explicit ThemeManager(ResolutionManager::Resolution resolution, QnxCar::Theme *theme, QObject *parent = 0);

    /**
     * Returns the absolute and themed path for the given @p iconName.
     */
    Q_INVOKABLE QUrl fromTheme(const QString& iconName) const;

    /**
     * Returns the assets directory, which is the base for the themes directory.
     */
    QString assetsDirectory() const;

    /**
     * Sets the assets directory, which is the base for the themes directory.
     */
    void setAssetsDirectory(const QString &assetsDirectory);

    QString path() const;

    /**
     * Return the Theme instance
     *
     * @note Never null
     */
    QnxCar::Theme *theme() const;

private:
    QString pathForTheme(const QString &theme, QnxCar::Theme::Mode mode) const;

    QnxCar::Theme *m_theme;

    QString m_assetsDirectory;
    ResolutionManager::Resolution m_resolution;
};

}

#endif
