#ifndef QPPS_SIMULATOR_OBJECT_P_H
#define QPPS_SIMULATOR_OBJECT_P_H

#include "object.h"

namespace QPps
{

class Object::Private : public QObject
{
    Q_OBJECT

public:
    Private(const QString &path, Object::PublicationMode mode, QObject *parent = 0);

    bool m_isValid;
    bool m_attributeCacheEnabled;
    QString m_ppsObjectPath;
    PublicationMode m_mode;
    QString m_errorString;

public Q_SLOTS:
    void notifyAttributeChanged(const QString &key, const QByteArray &value, const QByteArray &encoding);
    void notifyAttributeRemoved(const QString &key);

};

}

#endif
