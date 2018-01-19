#include "basecommand.h"

#include <QDebug>

namespace QPlayer {

BaseCommand::BaseCommand()
{
    connect(this, &BaseCommand::complete, this, &BaseCommand::deleteLater);
    connect(this, &BaseCommand::error, this, &BaseCommand::deleteLater);
}

BaseCommand::~BaseCommand()
{

}

void BaseCommand::setErrorMessage(QString error)
{
    m_errorMessage = error;
}

QString BaseCommand::errorMessage() const
{
    return m_errorMessage;
}

}
