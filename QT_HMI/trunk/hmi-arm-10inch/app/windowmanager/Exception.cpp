#include "Exception.h"

Exception::Exception(const QString &message)
    : std::runtime_error(message.toStdString())
    , m_message(message)
{
}

Exception::~Exception() throw()
{
}

QString Exception::message() const
{
    return m_message;
}
