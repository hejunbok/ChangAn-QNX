#ifndef STATUS_H
#define STATUS_H

#include <QObject>

/**
 * @short The class that provides data for the status bar
 */
class Status : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString dateTime READ dateTime NOTIFY dateTimeChanged)
    Q_PROPERTY(bool wifiActive READ wifiActive NOTIFY wifiActiveChanged)
    Q_PROPERTY(bool deviceActive READ deviceActive NOTIFY deviceActiveChanged)

public:
    /**
     * Creates a new Status object.
     *
     * @param parent The parent object.
     */
    explicit Status(QObject *parent = 0);

Q_SIGNALS:
    /**
     * This signal is emitted whenever the dateTime property has changed.
     */
    void dateTimeChanged();

    /**
     * This signal is emitted whenever the wifiActive property has changed.
     */
    void wifiActiveChanged();

    /**
     * This signal is emitted whenever the deviceActive property has changed.
     */
    void deviceActiveChanged();

private Q_SLOTS:
    void updateTime();

private:
    QString dateTime() const;
    bool wifiActive() const;
    bool deviceActive() const;

    QString m_dateTime;
    bool m_wifiActive;
    bool m_deviceActive;
};

#endif
