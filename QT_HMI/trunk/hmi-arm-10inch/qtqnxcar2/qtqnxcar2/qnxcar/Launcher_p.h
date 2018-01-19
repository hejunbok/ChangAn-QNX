#ifndef QTQNXCAR2_LAUNCHER_P_H
#define QTQNXCAR2_LAUNCHER_P_H

#include "Launcher.h"
#include "ApplicationListModel.h"

#include <qpps/changeset.h>
#include <qpps/object.h>

#include <QStringListModel>
#include <QRect>

namespace QnxCar {

class Launcher::Private : public QObject
{
public:
    Private(Launcher *qq = 0)
        : QObject(qq), q(qq)
        , applicationGroupModel(new QStringListModel(q))
        , applicationListModel(new ApplicationListModel(q))
        , appsInstallationDirectory(QStringLiteral("/apps"))
    {}

    Launcher *const q;

    QStringListModel *applicationGroupModel;
    ApplicationListModel *applicationListModel;
    QPps::Object *ppsApplications;
    QPps::Object *ppsLauncherControl;
    QPps::Object *ppsAppLauncher;
    QString appsInstallationDirectory;
    QRect m_applicationGeometry;
    QSize m_screenGeometry;

    /**
     * @brief removes applications from the list that populates the app-launcher pps object
     * @param applicationList the list of applications that will populate
     *        /pps/services/app-launcher
     *
     */
    QVector<ApplicationData> filterApplicationList(QVector<ApplicationData> applicationList);

public Q_SLOTS:
    void ppsLauncherReply(const QPps::Changeset &changes);
    void ppsApplicationsValuesChanged(const QPps::Changeset &changes);
};

}

#endif
