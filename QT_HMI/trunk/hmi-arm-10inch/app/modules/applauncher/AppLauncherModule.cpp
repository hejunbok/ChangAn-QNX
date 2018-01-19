#include "AppLauncherModule.h"

#include "ApplicationManager.h"
#include <Core.h>

#include "qnxcar/ApplicationFilterModel.h"
#include "qnxcar/ApplicationListModel.h"
#include "qnxcar/Launcher.h"

#include <QAbstractItemModel>
#include <QStringListModel>
#include <qqml.h>

const QString INVALID_GROUP = QStringLiteral("INTERNAL---INVALID_GROUP");

AppLauncherModule::AppLauncherModule(QObject *parent)
    : QObject(parent)
    , m_previousGroupFilterModel(new QnxCar::ApplicationFilterModel(this))
    , m_applicationFilterModel(new QnxCar::ApplicationFilterModel(this))
    , m_nextGroupFilterModel(new QnxCar::ApplicationFilterModel(this))
{
    qmlRegisterType<QnxCar::ApplicationFilterModel>();
    qmlRegisterType<QAbstractItemModel>();

    ApplicationManager *applicationManager = Core::self()->applicationManager();
//    const QStringList blacklist = QStringList() << QStringLiteral("system") << QStringLiteral("bridge") << QStringLiteral("mirrorlink")
//                                                << QStringLiteral("MirrorLink") << QStringLiteral("mlink") << QStringLiteral("carplay") << QStringLiteral("media");
    const QStringList blacklist = QStringList() << QStringLiteral("system") << QStringLiteral("bridge") << QStringLiteral("carplay");
    m_applicationFilterModel->setBlacklist(blacklist);
    m_applicationFilterModel->setSourceModel(applicationManager->launcher()->applicationListModel());
    m_applicationFilterModel->sort(0);
    connect(m_applicationFilterModel, SIGNAL(groupChanged(QString)), this, SIGNAL(currentGroupChanged(QString)));
    connect(m_applicationFilterModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SIGNAL(updateModel()));
    m_previousGroupFilterModel->setBlacklist(blacklist);
    m_previousGroupFilterModel->setSourceModel(applicationManager->launcher()->applicationListModel());
    m_previousGroupFilterModel->sort(0);
    connect(m_previousGroupFilterModel, SIGNAL(groupChanged(QString)), this, SIGNAL(previousGroupChanged(QString)));
    m_nextGroupFilterModel->setBlacklist(blacklist);
    m_nextGroupFilterModel->setSourceModel(applicationManager->launcher()->applicationListModel());
    m_nextGroupFilterModel->sort(0);
    connect(m_nextGroupFilterModel, SIGNAL(groupChanged(QString)), this, SIGNAL(nextGroupChanged(QString)));

    setCurrentGroup(QString());
}

void AppLauncherModule::moduleReset()
{
    ApplicationManager *applicationManager = Core::self()->applicationManager();
    applicationManager->stopApplication();
}

void AppLauncherModule::moduleSelected()
{
    ApplicationManager *applicationManager = Core::self()->applicationManager();
    applicationManager->showApplication();
}

void AppLauncherModule::moduleDeselected()
{
    ApplicationManager *applicationManager = Core::self()->applicationManager();
    applicationManager->hideApplication();
}

QnxCar::ApplicationFilterModel *AppLauncherModule::previousGroupFilterModel() const
{
    return m_previousGroupFilterModel;
}

QnxCar::ApplicationFilterModel *AppLauncherModule::applicationFilterModel() const
{
    return m_applicationFilterModel;
}

QnxCar::ApplicationFilterModel *AppLauncherModule::nextGroupFilterModel() const
{
    return m_nextGroupFilterModel;
}

QAbstractItemModel *AppLauncherModule::applicationGroupModel() const
{
    ApplicationManager *applicationManager = Core::self()->applicationManager();
    return applicationManager->launcher()->applicationGroupModel();
}

void AppLauncherModule::setCurrentGroup(const QString &group)
{
    if (group == INVALID_GROUP)
        return;

    const QAbstractItemModel *const groupModel = applicationGroupModel();
    Q_ASSERT(groupModel);

    const QModelIndexList indexes = groupModel->match(groupModel->index(0, 0), Qt::DisplayRole, group, 1, Qt::MatchExactly);

    if (indexes.isEmpty()) {
        qCritical("Group %s not found in model", qPrintable(group));
        return;
    }

    const QModelIndex idx = indexes.first();

    const QString previousGroup = idx.row() > 0 ? idx.sibling(idx.row()-1, 0).data().toString() :INVALID_GROUP;
    m_previousGroupFilterModel->setGroup(previousGroup);

    const QString nextGroup = (idx.row() < groupModel->rowCount() - 1) ? idx.sibling(idx.row()+1, 0).data().toString() : INVALID_GROUP;
    m_nextGroupFilterModel->setGroup(nextGroup);

    m_applicationFilterModel->setGroup(group);

    emit previousGroupChanged(previousGroup);
    emit nextGroupChanged(nextGroup);
    emit currentGroupChanged(group);
}

QString AppLauncherModule::currentGroup() const
{
    return m_applicationFilterModel->group();
}

QString AppLauncherModule::previousGroup() const
{
    return m_previousGroupFilterModel->group();
}

QString AppLauncherModule::nextGroup() const
{
    return m_nextGroupFilterModel->group();
}

QString AppLauncherModule::navigatorTabName() const
{
    return QStringLiteral("AppSection");
}
