#ifndef QNXCAR2UI_RESOLUTIONMANAGER_H
#define QNXCAR2UI_RESOLUTIONMANAGER_H

#include "qtqnxcar_ui_export.h"

#include <QObject>
#include <QRect>
#include <QString>

class QQmlContext;
class QPoint;
class QSize;

namespace QnxCarUi {

class QTQNXCAR_UI_EXPORT ResolutionManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Resolution resolution READ resolution CONSTANT)
    Q_PROPERTY(QRect externalApplicationGeometry READ externalApplicationGeometry CONSTANT)
    Q_PROPERTY(int heightForResolution READ heightForResolution CONSTANT)
    Q_PROPERTY(int widthForResolution READ widthForResolution CONSTANT)
    Q_ENUMS(Resolution)

public:
    enum Resolution {
        Resolution_800x480,
        Resolution_720p,
        Resolution_1920x1200,
        Resolution_1280x800
    };

    explicit ResolutionManager(QObject *parent = 0);

    /**
     * parses command-line arguments to enforce a certain resolution.
     * Currently supported are --resolution=800x480 and --resolution=720p.
     *
     * @return @c false in case an error occurred parsing the arguments. See errorString() then for details.
     */
    bool parseArguments(const QStringList &arguments);

    QString errorString() const;

    Resolution resolution() const;

    QSize sizeForResolution() const;

    int widthForResolution() const;

    int heightForResolution() const;

    QRect externalApplicationGeometry() const;

    int defaultPixelSize() const;

    void setupQmlContext(QQmlContext *context);

    QPoint mapPoint(const QPoint &p) const;

    /**
     * Maps a rectangle assuming 800x480 coordinates to the actual
     * resolution.
     *
     */
    QRect mapRect(const QRect &rect) const;

    QSize mapSize(const QSize &size) const;

    int mapPixels(int px) const;

    /**
     * The factor to multiply pixel measures with
     * to convert from 800x480 to the actual resolution.
     *
     * 1.0 for 800x480, 1.6 for 720p
     */
    qreal resolutionFactor() const;

    /**
     * The factor to multiply pixel Y (Height) measures with
     * to convert from 800x480 to the actual resolution, can be different from X just becasue we might work with different aspect ratio
     *
     * 1.0 for 800x480, 1.5 for 720p
     */
    qreal resolutionFactorY() const;

private:
    QString m_errorString;
    Resolution m_resolution;
};

}

#endif // QNXCAR2UI_RESOLUTIONMANAGER_H
