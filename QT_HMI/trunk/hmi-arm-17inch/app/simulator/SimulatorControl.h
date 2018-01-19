#ifndef SIMULATORCONTROL_H
#define SIMULATORCONTROL_H

#include <QObject>

class QByteArray;

class SimulatorControl : public QObject
{
    Q_OBJECT

public:
    explicit SimulatorControl(QObject *parent = 0);

    /**
     * @p service One of ["hfp", "map", "pbap"]
     */
    void setBluetoothDeviceEnabled(const QString &service, bool enabled);

private Q_SLOTS:
    void ppsAttributeChanged(const QString &objectPath, const QString &key, const QByteArray &value, const QByteArray &encoding);

    void onHandsFreeCallOutgoingDialed();

private:
    void updateBluetoothStatus(bool status);
    void updateHandsFreeStatus();
};

#endif
