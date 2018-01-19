#ifndef QTQNXCAR2_THEMECONTROL_H
#define QTQNXCAR2_THEMECONTROL_H

#include <QObject>

#include "qtqnxcar2_export.h"

namespace QnxCar {

/**
 * Access to heating, ventilation and air conditioning
 *
 * Wraps the @c /pps/qnxcar/navigation/lampstatus PPS object
 */
class QTQNXCAR2_EXPORT ThemeControl : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString getCurrentTheme READ getCurrentTheme WRITE setCurrentTheme NOTIFY currentThemeChanged)

public:
    /**
     * Creates a new LampStatus object.
     *
     * @param parent The parent object.
     */
    explicit ThemeControl(QObject *parent = 0);

    QString getCurrentTheme() const;
    Q_INVOKABLE void setCurrentTheme(QString currentTheme);

Q_SIGNALS:
    void currentThemeChanged(QString);

private:
    class Private;
    Private* const d;
};

}

#endif
