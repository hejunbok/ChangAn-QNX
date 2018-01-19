#ifndef APPLAUNCHERMODULE_H
#define APPLAUNCHERMODULE_H

#include "AbstractModule.h"

#include <QObject>

namespace QnxCar {
    class ApplicationFilterModel;
}

class QAbstractItemModel;

class ApplicationManager;

class AppLauncherModule : public QObject, public AbstractModule
{
    Q_OBJECT

    Q_PROPERTY(QnxCar::ApplicationFilterModel *previousGroupFilterModel READ previousGroupFilterModel CONSTANT)
    Q_PROPERTY(QnxCar::ApplicationFilterModel *applicationFilterModel READ applicationFilterModel CONSTANT)
    Q_PROPERTY(QnxCar::ApplicationFilterModel *nextGroupFilterModel READ nextGroupFilterModel CONSTANT)
    Q_PROPERTY(QAbstractItemModel *applicationGroupModel READ applicationGroupModel CONSTANT)
    Q_PROPERTY(QString currentGroup READ currentGroup WRITE setCurrentGroup NOTIFY currentGroupChanged)
    Q_PROPERTY(QString previousGroup READ previousGroup NOTIFY previousGroupChanged)
    Q_PROPERTY(QString nextGroup READ nextGroup NOTIFY nextGroupChanged)

public:
    explicit AppLauncherModule(QObject *parent = 0);

    virtual void moduleReset() Q_DECL_OVERRIDE;
    virtual void moduleSelected() Q_DECL_OVERRIDE;
    virtual void moduleDeselected() Q_DECL_OVERRIDE;
    virtual QString navigatorTabName() const Q_DECL_OVERRIDE;

    QString currentGroup() const;
    void setCurrentGroup(const QString &group);

    QString previousGroup() const;
    QString nextGroup() const;

Q_SIGNALS:
    void updateModel();
    void currentGroupChanged(const QString &currentGroup);
    void previousGroupChanged(const QString &previousGroup);
    void nextGroupChanged(const QString &nextGroup);

private:
    QnxCar::ApplicationFilterModel *applicationFilterModel() const;
    QnxCar::ApplicationFilterModel *previousGroupFilterModel() const;
    QnxCar::ApplicationFilterModel *nextGroupFilterModel() const;

    QAbstractItemModel *applicationGroupModel() const;

private:
    QnxCar::ApplicationFilterModel *m_previousGroupFilterModel;
    QnxCar::ApplicationFilterModel *m_applicationFilterModel;
    QnxCar::ApplicationFilterModel *m_nextGroupFilterModel;
};

#endif
