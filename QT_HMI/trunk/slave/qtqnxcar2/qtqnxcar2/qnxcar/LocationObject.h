#ifndef QTQNXCAR2_LOCATIONOBJECT_H
#define QTQNXCAR2_LOCATIONOBJECT_H

#include "qtqnxcar2_export.h"

#include "qnxcar/Location.h"

#include <QObject>

namespace QnxCar {

/**
 * Wrapper QObject class to expose Location objects to QML
 */
class QTQNXCAR2_EXPORT LocationObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isValid READ isValid NOTIFY isValidChanged)
    Q_PROPERTY(int locationId READ locationId NOTIFY locationIdChanged)
    Q_PROPERTY(QString name READ name NOTIFY nameChanged)
    Q_PROPERTY(QString type READ type NOTIFY typeChanged)
    Q_PROPERTY(QString street READ street NOTIFY streetChanged)
    Q_PROPERTY(QString streetNumber READ streetNumber NOTIFY streetNumberChanged)
    Q_PROPERTY(QString postalCode READ postalCode NOTIFY postalCodeChanged)
    Q_PROPERTY(QString city READ city NOTIFY cityChanged)
    Q_PROPERTY(QString province READ province NOTIFY provinceChanged)
    Q_PROPERTY(QString country READ country NOTIFY countryChanged)
    Q_PROPERTY(int distance READ distance NOTIFY distanceChanged)
    Q_PROPERTY(qreal latitude READ latitude NOTIFY latitudeChanged)
    Q_PROPERTY(qreal longitude READ longitude NOTIFY longitudeChanged)
    Q_PROPERTY(QString shortLabel READ shortLabel NOTIFY shortLabelChanged)

public:
    explicit LocationObject(QObject *parent = 0);
    ~LocationObject();

    bool isValid() const;
    int locationId() const;
    QString name() const;
    QString type() const;
    QString street() const;
    QString streetNumber() const;
    QString postalCode() const;
    QString city() const;
    QString province() const;
    QString country() const;
    int distance() const;
    qreal latitude() const;
    qreal longitude() const;

    /**
     * Label used to describe the location,
     *
     */
    QString shortLabel() const;

Q_SIGNALS:
    void isValidChanged(bool);
    void locationIdChanged(int);
    void nameChanged(const QString &);
    void typeChanged(const QString &);
    void streetChanged(const QString &);
    void streetNumberChanged(const QString &);
    void postalCodeChanged(const QString &);
    void cityChanged(const QString &);
    void provinceChanged(const QString &);
    void countryChanged(const QString &);
    void distanceChanged(int);
    void latitudeChanged(qreal);
    void longitudeChanged(qreal);
    void shortLabelChanged(const QString &shortLabel);

public Q_SLOTS:
    void setLocation(const QnxCar::Location &location);

private:
    class Private;
    Private *const d;
};

}

#endif
