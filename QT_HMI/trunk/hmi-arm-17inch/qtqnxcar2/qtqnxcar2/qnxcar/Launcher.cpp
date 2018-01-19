#include "Launcher.h"
#include "Launcher_p.h"

#include <QDebug>
#include <QSize>
#include <QVariantMap>
#include <QJsonArray>
#include <QJsonDocument>

namespace QnxCar {

static const char* PPS_MSG_KEY = "msg";
static const char* PPS_RES_KEY = "res";
static const char* PPS_DAT_KEY = "dat";
static const char* PPS_ID_KEY = "id";
static const char* PPS_ERR_KEY = "err";
static const char* PPS_ERR_MEG_KEY = "errmsg";

/* Structure to represent a message*/
struct Msg {
    QString msg;
    QString res;
    QString dat;
    QString id;
    QString errorcode;
    QString error;
};

/**
 * A helper method to extract the icon name and size from the entries of
 * the /pps/system/navigator/applications/applications file.
 */
static void splitInSizeAndName(const QString &sizeAndName, QSize &size, QString &name)
{
    name = sizeAndName; //if we don't find the size prepended, interpret as icon path

    // The entries look like '{128x128}someIcon.png' or just 'anotherIcon.png'
    QRegExp re(QStringLiteral("\\{(\\d+)x(\\d+)\\}(.+)"));
    if (!re.exactMatch(sizeAndName))
        return;

    if (re.captureCount() != 3)
        return;

    const QString widthStr = re.cap(1);
    const QString heightStr = re.cap(2);
    const QString path = re.cap(3);

    bool ok;
    const int width = widthStr.toInt(&ok);
    if (!ok)
        return;

    const int height = heightStr.toInt(&ok);
    if (!ok)
        return;

    size = QSize(width, height);
    name = path;
}

Launcher::Launcher(QObject *parent)
    : QObject(parent)
    , d(new Private(this))
{

    d->ppsAppLauncher = new QPps::Object(QStringLiteral("/pps/services/app-launcher"),
                                         QPps::Object::PublishMode, true, this);

    d->ppsApplications = new QPps::Object(QStringLiteral("/pps/system/navigator/applications/applications"),
                                          QPps::Object::SubscribeMode, true, this);

    d->ppsLauncherControl = new QPps::Object(QStringLiteral("/pps/services/launcher/control"),
                                             QPps::Object::PublishAndSubscribeMode, true, this);

    if (!d->ppsAppLauncher->isValid()){
        qCritical("%s Could not open %s: %s", Q_FUNC_INFO, qPrintable(d->ppsAppLauncher->path()), qPrintable(d->ppsAppLauncher->errorString()));
    }

    if (d->ppsApplications->isValid()) {
        connect(d->ppsApplications, &QPps::Object::attributesChanged,
                d, &Private::ppsApplicationsValuesChanged);
        d->ppsApplications->setAttributeCacheEnabled(true);
    } else {
        qCritical("%s Could not open %s: %s", Q_FUNC_INFO, qPrintable(d->ppsApplications->path()), qPrintable(d->ppsApplications->errorString()));
    }

    if (d->ppsLauncherControl->isValid()) {
        connect(d->ppsLauncherControl, &QPps::Object::attributesChanged,d, &Private::ppsLauncherReply);
        d->ppsLauncherControl->setAttributeCacheEnabled(true);
    } else {
        qCritical("%s Could not open %s: %s", Q_FUNC_INFO, qPrintable(d->ppsLauncherControl->path()), qPrintable(d->ppsLauncherControl->errorString()));
    }

//    const QStringList groups = QStringList() << QString() << QStringLiteral("vehicle") << QStringLiteral("games") << QStringLiteral("media") << QStringLiteral("internet");
    const QStringList groups = QStringList() << QString() << QStringLiteral("车辆") << QStringLiteral("游戏") << QStringLiteral("多媒体") << QStringLiteral("因特网");
    d->applicationGroupModel->setStringList(groups);
}

Launcher::~Launcher()
{
    delete d;
}

void Launcher::Private::ppsLauncherReply(const QPps::Changeset &changes) {
    QMap<QString,QPps::Variant>::ConstIterator ppsIt;

    Msg message;

    for (ppsIt = changes.assignments.constBegin(); ppsIt != changes.assignments.constEnd(); ++ppsIt) {

        QString key = ppsIt.key();

        if(key == QString::fromLatin1(PPS_MSG_KEY)) {
            message.msg = ppsIt.value().toString();
        } else if(key == QString::fromLatin1(PPS_RES_KEY)) {
            message.res = ppsIt.value().toString();
        } else if(key == QString::fromLatin1(PPS_ID_KEY)) {
            message.id = ppsIt.value().toString();
        } else if(key == QString::fromLatin1(PPS_DAT_KEY)) {
            message.dat = ppsIt.value().toString();
        } else if(key == QString::fromLatin1(PPS_ERR_KEY)) {
            message.errorcode = ppsIt.value().toString();
        }
    }

    // Get the PID in the response
    // Note that this is not platform independant and assumes that the Q_PID data type is a long long
    Q_PID pid = 0;

    if(!message.dat.isEmpty()) {
        bool ok = 0;
        // Split dat::1234567890,dev_mode to obtain PID. dev_mode isn't always present, but the PID is
        // always the first element.
        QString pidStr = message.dat.split(",")[0];
        pid = pidStr.toLongLong(&ok);

        if(!ok) {
            qWarning("%s: Could not convert dat to PID. dat: %s", Q_FUNC_INFO, qPrintable(message.dat));
        }
    }

    if(message.msg == QStringLiteral("stopped")) {
        // Application has stopped
        emit q->applicationStopped(pid);
    } else if(message.res == QStringLiteral("start")) {
        // Application either started successfully, or there was an error when attempting to start
        if(message.errorcode != NULL && !message.errorcode.isNull() && !message.errorcode.isEmpty()) {
            // An error is present, which means the application has failed to start
           qWarning("%s: Application failed to start", Q_FUNC_INFO);
           emit q->applicationStartFailed();
        } else if (message.dat != NULL && !message.dat.isNull() && !message.dat.isEmpty()) {
            emit q->applicationStarted(pid);
        }
    }
}

void Launcher::Private::ppsApplicationsValuesChanged(const QPps::Changeset &changes)
{
    QMap<QString, QPps::Variant>::ConstIterator it = changes.assignments.constBegin();

    for (; it != changes.assignments.constEnd(); ++it) {
        const QPps::Variant attribute = it.value();
        const QStringList list = attribute.toString().split(QLatin1Char(','));
        if (list.size() < 7) {
            qWarning() << "Could not parse application info from" << attribute.toString();
            return;
        }

        const QString iconWithSize = list[0];
        QSize iconSize;
        QString iconName;

        splitInSizeAndName(iconWithSize, iconSize, iconName);
        const QString iconPath = QStringLiteral("%1/%2/%3").arg(appsInstallationDirectory, it.key(), iconName);

        ApplicationData appData;
        appData.setId(it.key());
        appData.setName(list[1]);
        appData.setGroup(list[2]);
        appData.setIconPath(iconPath);
        applicationListModel->addApplication(appData);
    }

    Q_FOREACH(const QString &key, changes.removals) {
        applicationListModel->removeApplication(key);
    }

    //populate the /pps/services/app-launcher with the currently installed apps
    //asr will read this list of apps and build up an internal map of apps
    //that can be launched. The asr will only build the app list once when it starts up.
    //Any app installed later will be ignored.

    //Here we filter out some app that we don't want ASr to be able to launch such as Navigator
    //and and the old Media Player. This should really be done somewhere else but becuase of how
    //the hmi is architected this will do until its changed
    QVector<ApplicationData> appList = filterApplicationList(applicationListModel->applications());

    QJsonArray jsonArray;
    for (int i = 0; i < appList.size(); i++) {
        jsonArray.append(appList.at(i).name());
    }

    ppsAppLauncher->setAttribute(QStringLiteral("app_list"), QPps::Variant(QJsonDocument(jsonArray)));

}

/**
 * A helper method to filter out the apps that we don't want to launch
 * from the list that populates the /pps/services/app-launcher object
 */
QVector<ApplicationData> Launcher::Private::filterApplicationList( QVector<ApplicationData> applicationList ){
    QList<QString> filterList;
    filterList<<"Navigator.testDev_Navigator__a4514a37"<<"MediaPlayer.testDev_MediaPlayer49ba23c5";

    for(int i = 0; i < filterList.size(); i++){
        QString app = applicationListModel->findById(filterList[i]).id();

        for (int j = 0; j < applicationList.size(); j++){
            if(app == applicationList.at(j).id()){
                applicationList.removeAt(j);
                break;
            }
        }
    }
    return applicationList;
}

bool Launcher::start(const QString &appId)
{
    QPps::Changeset changeset;
    changeset.assignments.insert(QStringLiteral("msg"), QPps::Variant(QStringLiteral("start")));
    changeset.assignments.insert(QStringLiteral("id"), QPps::Variant(QStringLiteral("1")));
    //if application geometry not set, send launch command without WIDTH, HEIGHT
    if(!d->m_applicationGeometry.isNull()) {

        int tabBarHeight = d->m_screenGeometry.height() - ( d->m_applicationGeometry.y() + d->m_applicationGeometry.height() );

        changeset.assignments.insert(QStringLiteral("dat"), QPps::Variant(appId + QStringLiteral(",ORIENTATION=0")
                                                                          + QStringLiteral(",WIDTH=") + QString::number(d->m_applicationGeometry.width())
                                                                          + QStringLiteral(",HEIGHT=") + QString::number(d->m_applicationGeometry.height())
                                                                          + QStringLiteral(",CAR_TABBAR_HEIGHT=") + QString::number(tabBarHeight)));
    } else {
        qWarning("Apllication geometry not set when starting application: %s", qPrintable(appId));
        changeset.assignments.insert(QStringLiteral("dat"), QPps::Variant(appId + QStringLiteral(",ORIENTATION=0")));
    }

    if (!d->ppsLauncherControl->setAttributes(changeset)) {
        qCritical("Could not write start to launcher control: %s", qPrintable(d->ppsLauncherControl->errorString()));
        return false;
    }
    return true;
}

bool Launcher::stop(const QString &appId)
{
    QPps::Changeset changeset;
    changeset.assignments.insert(QStringLiteral("msg"), QPps::Variant(QStringLiteral("stop")));
    changeset.assignments.insert(QStringLiteral("id"), QPps::Variant(QStringLiteral("1")));
    changeset.assignments.insert(QStringLiteral("dat"), QPps::Variant(appId));

    if (!d->ppsLauncherControl->setAttributes(changeset)) {
        qCritical("Could not write stop to launcher control: %s", qPrintable(d->ppsLauncherControl->errorString()));
        return false;
    }
    return true;
}

bool Launcher::stop(Q_PID pid)
{
    //Trying to the PID as string for all platforms
    //Not that it would matter much, for windows
#ifdef Q_OS_WIN
    QString pidStr;
    pidStr.sprintf("%08p", pid);
#else
    const QString pidStr = QString::number(pid);
#endif
    QPps::Changeset changeset;
    changeset.assignments.insert(QStringLiteral("msg"), QPps::Variant(QStringLiteral("stop")));
    changeset.assignments.insert(QStringLiteral("id"), QPps::Variant(QStringLiteral("1")));
    changeset.assignments.insert(QStringLiteral("dat"), QPps::Variant(pidStr));

    if (!d->ppsLauncherControl->setAttributes(changeset)) {
        qCritical("Could not write stop to launcher control: %s", qPrintable(d->ppsLauncherControl->errorString()));
        return false;
    }
    return true;
}

#if 0
enum AppLauncherCommand {
    Launch,
    Close,
};
static QVariantMap buildAppLauncherRequest(const QString &appName, AppLauncherCommand cmd)
{
    QVariantMap map;
    map[QStringLiteral("app")] = appName;
    map[QStringLiteral("cmd")] = (cmd == Launch ? QStringLiteral("launch app") : QStringLiteral("close app"));
    map[QStringLiteral("dat")] = QStringLiteral("");
    map[QStringLiteral("id")] = appName;
    return map;
}
#endif

bool Launcher::launch(const QString &appName)
{
    if (appName.isEmpty())
        return false;

    // WARNING: /pps/services/app-launcher is supposed to be monitored by the client application!
    // At least that's what the original version does.
    // TODO: Evaluate if we want to implement monitoring /pps/services/app-launcher
#if 0
    QJsonDocument request = QJsonDocument::fromVariant(buildAppLauncherRequest(appName, Launch));
    return m_ppsAppLauncher->setAttribute(QStringLiteral("req"), QPps::Variant(request));

#else
    // map from appName to appId ourselves
    const ApplicationData appData = d->applicationListModel->findByName(appName);
    if (!appData.isValid())
        return false;

    return start(appData.id());
#endif
}

bool Launcher::close(const QString &appName)
{
    if (appName.isEmpty())
        return false;

#if 0
    QJsonDocument request = QJsonDocument::fromVariant(buildAppLauncherRequest(appName, Close));
    return m_ppsAppLauncher->setAttribute(QStringLiteral("req"), QPps::Variant(request));
#else
    // map from appName to appId ourselves
    const ApplicationData appData = d->applicationListModel->findByName(appName);
    if (!appData.isValid())
        return false;

    return stop(appData.id());
#endif
}

ApplicationListModel *Launcher::applicationListModel() const
{
    return d->applicationListModel;
}

QStringListModel *Launcher::applicationGroupModel() const
{
    return d->applicationGroupModel;
}

QString Launcher::appsInstallationDirectory() const
{
    return d->appsInstallationDirectory;
}

void Launcher::setAppsInstallationDirectory(const QString &appsInstallationDirectory)
{
    d->appsInstallationDirectory = appsInstallationDirectory;
}

void Launcher::setApplicationGeometry(const QRect applicationGeometry)
{
    d->m_applicationGeometry = applicationGeometry;
}

void Launcher::setScreenGeometry(const QSize screenGeometry)
{
    d->m_screenGeometry = screenGeometry;
}

}
