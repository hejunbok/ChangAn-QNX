#ifndef QTQNXCAR2_NAVIGATIONSETTINGSMODELS_H
#define QTQNXCAR2_NAVIGATIONSETTINGSMODELS_H

#include "qtqnxcar2_export.h"

#include "SqlQueryModel.h"

namespace QnxCar {

/**
 * Model listing the navigation settings of the current profile
 *
 * This model manages the configurable options set by the user
 */
class QTQNXCAR2_EXPORT NavigationSettingsModel : public SqlQueryModel
{
    Q_OBJECT

    /**
     * Identifier of the currently active profile
     *
     * @accessors activeProfileId(), setActiveProfileId()
     * @sa ProfileModel
     */
    Q_PROPERTY(int activeProfileId READ activeProfileId WRITE setActiveProfileId)

public:
    /**
     * Creates an empty model
     *
     * Initial data filling happens on setDatabase()
     *
     * @param parent the QObject parent
     */
    explicit NavigationSettingsModel(QObject* parent = 0);

    /**
     * Returns the identifier for the currently active profile
     *
     * @sa #activeProfileId
     */
    int activeProfileId() const;

    /**
     * Sets the given profile identifier to be the current one
     * @param profileId the profile to activate
     *
     * @sa #activeProfileId
     */
    void setActiveProfileId(int profileId);

    /**
     * Sets a given option with specified key to specified value
     *
     * @param key of setting to change
     * @param value of setting to change
     */
    Q_INVOKABLE void addNavigationSetting(const QString key, const QString value);

    /**
     * Gets the value of a setting with the specified key. Returns empty if not found.
     *
     * @param key of a setting
     */
    Q_INVOKABLE QString valueForKey(const QString key);

protected:
    QList<QByteArray> columnNames() const;
    void runQuery();

private:
    class Private;
    Private* const d;
};

}

#endif // QTQNXCAR2_NAVIGATIONSETTINGSMODELS_H

