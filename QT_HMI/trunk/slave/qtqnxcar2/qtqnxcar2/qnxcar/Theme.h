#ifndef QTQNXCAR2_THEME_H
#define QTQNXCAR2_THEME_H

#include <QObject>
#include <QUrl>

#include "qtqnxcar2_export.h"

namespace QnxCar {

/**
 * A wrapper class for the @c /pps/qnxcar/profile/theme PPS object
 */
class QTQNXCAR2_EXPORT Theme : public QObject
{
    Q_OBJECT

    /// The current theme that is used
    Q_PROPERTY(QString current READ current WRITE setCurrent NOTIFY themeChanged)

    /// The current mode that is used
    Q_PROPERTY(QnxCar::Theme::Mode mode READ mode WRITE setMode NOTIFY modeChanged)

    Q_ENUMS(Theme)
    Q_ENUMS(Mode)

public:
    /// Describes the available modes
    enum Mode
    {
        Day,    ///< A brighter theme for good visibility during the day
        Night   ///< A darker theme for less distracting visibility during night
    };

    /**
     * Creates a new Theme object.
     *
     * @param parent The parent object.
     */
    explicit Theme(QObject *parent = 0);

    /**
     * Returns the identifier of the default theme.
     */
    static QString defaultTheme();

    /// #current
    QString current() const;
    /// #current
    void setCurrent(const QString& theme);

    /// #mode
    Mode mode() const;
    /// #mode
    void setMode(Mode mode);

Q_SIGNALS:
    /// This signal is emitted whenever the current theme has changed
    void themeChanged(const QString &theme);

    /// This signal is emitted whenever the current mode has changed
    void modeChanged(QnxCar::Theme::Mode mode);

private:
    class Private;
    Private* const d;
};

}

Q_DECLARE_METATYPE(QnxCar::Theme::Mode);

#endif
