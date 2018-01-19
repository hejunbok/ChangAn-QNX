#ifndef UTIL_H
#define UTIL_H

#include <QObject>

class QUrl;

class Util : public QObject
{
    Q_OBJECT

public:
    explicit Util(QObject* parent = 0);

    Q_INVOKABLE bool fileExists(const QUrl &url) const;
};

#endif
