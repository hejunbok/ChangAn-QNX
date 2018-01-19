#ifndef QTQNXCAR2_SYSTEMINFO_H
#define QTQNXCAR2_SYSTEMINFO_H

#include "qtqnxcar2_export.h"

#include <QObject>
#include <QString>
#include <qpps/variant.h>
#include <qpps/changeset.h>

namespace QnxCar {

/**
 * Provides system information
 *
 * This class provides general system information such
 * as the software #revision. It also allows to perform
 * software updates.
 *
 * Wrapper around PPS objects:
 * - system: @c /pps/qnxcar/system/
 * - update service status: @c /pps/services/update/status
 * - update service control: @c /pps/services/update/control
 *
 * TODO: This class has to be redesigned to match same D-Pointer style as other services.
 *
 */
class QTQNXCAR2_EXPORT SystemInfo : public QObject
{
    Q_OBJECT

    /**
     * The system image's build number
     *
     * @accessors buildNumber
     * @sa #buildDate
     */
    Q_PROPERTY(QString buildNumber READ buildNumber CONSTANT)

    /**
     * The system's revision string
     *
     * @accessors revision
     */
    Q_PROPERTY(QString revision READ revision CONSTANT)

    /**
     * The system's build date

     * @accessors buildDate()
     * @sa #buildNumber
     */
    Q_PROPERTY(QString buildDate READ buildDate CONSTANT)

    /**
     * Whether there are any updates available
     *
     * @accessors updateAvailable
     */
    Q_PROPERTY(bool updateAvailable READ updateAvailable NOTIFY updateAvailableChanged)

    /**
     * Contains update details
     *
     * @accessors updateDetails
     */
    Q_PROPERTY(QString updateDetails READ updateDetails NOTIFY updateDetailsChanged)

    /**
     * Contains error message when there is something wrong with provided update
     *
     * @accessors updateError
     */
    Q_PROPERTY(QString updateError READ updateError NOTIFY updateErrorChanged)

    /**
     * Indicates if Software Update status available, 'true' available, 'false' not available
     *
     * @accessors isAvailable
     */
    Q_PROPERTY(bool available READ isAvailable NOTIFY availableChanged)

public:
    /**
     * Conntects to PPS and retrieves initial values
     *
     * @param parent the QObject parent
     */
    explicit SystemInfo(QObject *parent = 0);
    ~SystemInfo();

    /**
     * #buildNumber
     * @brief returns build number
     * Function returns current build number
     * @return build number string for example 1784
     */
    QString buildNumber() const;

    /**
     * #revision
     * @brief retuns current revision
     * Function return curent revision
     * @return current revision string
     */
    QString revision() const;

    /**
     * #buildDate
     * @brief returns currentbuild date
     * Function returns currentbuild date
     * @return
     */
    QString buildDate() const;

    /**
     * #updateAvailable
     * @brief update available
     * Function returns true is update is available
     * @return true if update available
     */
    bool updateAvailable() const;

    /**
     * @brief update details
     * Function returns update details
     * @return update details string
     */
    QString updateDetails() const;

    /**
     * @brief update error
     * Function returns update error
     * @return update error string
     */
    QString updateError() const;

    /**
     * @brief return true if SFWUD available
     * Function returns 'true' if SFWUD available, 'false' if not to whatewer reason.
     * @return true if SFWUD available
     */
    bool isAvailable() const;

    /**
     * @brief perform update
     * Request an update through the update service.
     */
    Q_INVOKABLE void performUpdate();

signals:

    /**
     * @brief update availabliability changed
     * Signals that there update available changed
     * @param updateAvailable true if update is available
     */
    void updateAvailableChanged(bool updateAvailable);

    /**
     * @brief update details changed
     * Signals that there is new update details availalbe
     */
    void updateDetailsChanged();

    /**
     * @brief update Error Changed
     * Signals that tehre is an update error
     */
    void updateErrorChanged();

    /**
     * @brief signals that availablity changed
     * Signals when SWUD availability changed
     * @param value indicates if SWUD available or not
     */
    void availableChanged(bool available);

private slots:
    /**
     * @brief handleUpdateInfoChanged
     * @param changes
     */
    void handleUpdateInfoChanged(const QPps::Changeset &changes);
    /**
     * @brief setAvailable
     * @param value
     */
    void setAvailable(bool value);

private:
    class Private;
    Private *const d;
};

}

#endif
