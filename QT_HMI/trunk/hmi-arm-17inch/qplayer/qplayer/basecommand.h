#ifndef BASECOMMAND_H
#define BASECOMMAND_H

#include <QObject>

namespace QPlayer {

class BaseCommand : public QObject
{
    Q_OBJECT
public:
    explicit BaseCommand();
    ~BaseCommand();

    void setErrorMessage(QString message);
    QString errorMessage() const;

signals:
    void complete(BaseCommand *command);    // Must be overridden in derived class
    void error(BaseCommand *command);       // Must be overridden in derived class

private:
    QString m_errorMessage;
};

}

#endif // BASECOMMAND_H
