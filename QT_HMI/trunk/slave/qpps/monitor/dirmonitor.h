#ifndef DIRMONITOR_H
#define DIRMONITOR_H

#include <QByteArray>
#include <QList>
#include <QHash>
#include <QObject>
#include <QString>

class QSocketNotifier;

class DirMonitor : public QObject
{
    Q_OBJECT
public:
    explicit DirMonitor(const QString &path, QObject *parent = 0);

private slots:
    void readData();

private:
    bool prettyPrintLines(const QList<QByteArray> &lines);

    // key: object name
    // value:
    //   key: attribute name
    //   value: attribute value (everything behind the name, not parsed)
    QHash<QString, QHash<QString, QString> > m_seenAttributes;
    QString m_path;
    QSocketNotifier *m_notifier;
};

#endif // DIRMONITOR_H
