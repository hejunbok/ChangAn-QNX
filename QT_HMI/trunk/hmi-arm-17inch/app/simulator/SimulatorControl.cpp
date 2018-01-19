#include "SimulatorControl.h"

#include "util/Util.h"

#include <qpps/simulator.h>
#include <qpps/object.h>

#include <QString>
#include <QStringList>
#include <QTimer>

static const QString PPS_HANDSFREECONTROL = QStringLiteral("/pps/services/handsfree/control");
static const QString PPS_HANDSFREESTATUS = QStringLiteral("/pps/services/handsfree/status");
static const QString PPS_BLUETOOTHSETTINGS = QStringLiteral("/pps/services/bluetooth/settings");
static const QString PPS_BLUETOOTHSERVICES = QStringLiteral("/pps/services/bluetooth/services");

static const QStringList BLUETOOTH_SERVICE_LIST = QStringList()
    << QStringLiteral("hfp") << QStringLiteral("map") << QStringLiteral("pbap");

/// If any of the bluetooth services has a valid device, return true
static bool isBluetoothEnabled()
{
    QPps::Object object(PPS_BLUETOOTHSERVICES);
    object.setAttributeCacheEnabled(true);
    Q_FOREACH (const QString &service, BLUETOOTH_SERVICE_LIST) {
        if (!object.attribute(service).toString().isEmpty())
            return true;
    }
    return false;
}

SimulatorControl::SimulatorControl(QObject *parent)
    : QObject(parent)
{
    connect(QPps::Simulator::self(), SIGNAL(attributeChanged(QString,QString,QByteArray,QByteArray)),
            this, SLOT(ppsAttributeChanged(QString,QString,QByteArray,QByteArray)));
}

void SimulatorControl::ppsAttributeChanged(const QString &objectPath, const QString &key, const QByteArray &value, const QByteArray &)
{
    Q_UNUSED(value)
    if (objectPath == QStringLiteral("/pps/services/handsfree/control")
            && key == QStringLiteral("command")) {
        updateHandsFreeStatus();
    }
}

void SimulatorControl::setBluetoothDeviceEnabled(const QString &service, bool enabled)
{
    static const QByteArray fakeDevice("DD:03:35:4E:98:40");

    Q_ASSERT(BLUETOOTH_SERVICE_LIST.contains(service));

    QPps::Simulator * const simulator = QPps::Simulator::self();
    simulator->registerClient(PPS_BLUETOOTHSERVICES, this);
    simulator->clientSetAttribute(this, service, (enabled ? fakeDevice : QByteArray()), QByteArray());
    simulator->unregisterClient(this);

    updateBluetoothStatus(isBluetoothEnabled());
}

void SimulatorControl::updateBluetoothStatus(bool status)
{
    QPps::Simulator * const simulator = QPps::Simulator::self();

    simulator->registerClient(PPS_BLUETOOTHSETTINGS, this);
    simulator->clientSetAttribute(this, QStringLiteral("active_connections"),
                                  QByteArray(status ? "true" : "false"), QByteArrayLiteral("b"));
    simulator->unregisterClient(this);

    simulator->registerClient(PPS_HANDSFREESTATUS, this);
    simulator->clientSetAttribute(this, QStringLiteral("state"),
                                  QByteArray(status ? "HFP_CONNECTED_IDLE" : "HFP_INITIALIZED"), QByteArray());
    simulator->unregisterClient(this);

    simulator->registerClient(PPS_HANDSFREECONTROL, this);
    simulator->clientSetAttribute(this, QStringLiteral("command"), QByteArray(), QByteArray());
    simulator->clientSetAttribute(this, QStringLiteral("cmd_data"), QByteArray(), QByteArray());
    simulator->unregisterClient(this);
}

void SimulatorControl::updateHandsFreeStatus()
{
    // result buffers:
    QByteArray command;
    QByteArray encoding;

    QPps::Simulator * const simulator = QPps::Simulator::self();

    simulator->registerClient(PPS_HANDSFREECONTROL, this);
    simulator->clientGetAttribute(this, QStringLiteral("command"), command, encoding);
    simulator->unregisterClient(this);

    if (command == QByteArrayLiteral("HFP_CALL")) {
        simulator->registerClient(PPS_HANDSFREESTATUS, this);
        simulator->clientSetAttribute(this, QStringLiteral("cmd_status"),
                                      QByteArrayLiteral("HFP_SUCCESS"), QByteArray());
        simulator->clientSetAttribute(this, QStringLiteral("state"),
                                      QByteArrayLiteral("HFP_CALL_OUTGOING_DIALING"), QByteArray());
        simulator->unregisterClient(this);

        // Let's assume dialing takes a second and always succeeds:
        QTimer::singleShot(1000, this, SLOT(onHandsFreeCallOutgoingDialed()));
    } else if (command == QByteArrayLiteral("HFP_HANGUP")) {
        simulator->registerClient(PPS_HANDSFREESTATUS, this);
        simulator->clientSetAttribute(this, QStringLiteral("cmd_status"),
                                      QByteArrayLiteral("HFP_SUCCESS"), QByteArray());
        simulator->clientSetAttribute(this, QStringLiteral("state"),
                                      QByteArrayLiteral("HFP_CONNECTED_IDLE"), QByteArray());
        simulator->unregisterClient(this);
    }
}

void SimulatorControl::onHandsFreeCallOutgoingDialed()
{
    QPps::Simulator * const simulator = QPps::Simulator::self();

    simulator->registerClient(PPS_HANDSFREESTATUS, this);
    simulator->clientSetAttribute(this, QStringLiteral("state"),
                                  QByteArrayLiteral("HFP_CALL_ACTIVE"), QByteArray());
    simulator->unregisterClient(this);
}
