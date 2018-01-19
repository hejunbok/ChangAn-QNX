#include "CallerID_p.h"
#include "CallerIDObject.h"

using namespace QnxCar;

class CallerIDObject::Private
{
public:
    CallerID callerID;
};

CallerIDObject::CallerIDObject(QObject *parent) :
    QObject(parent),
    d(new Private)
{
}


CallerIDObject::~CallerIDObject()
{
    delete d;
}


void CallerIDObject::setCallerID(const QnxCar::CallerID &callerID)
{
    d->callerID = callerID;
    emit changed();
}


CallerID QnxCar::CallerIDObject::callerID() const
{
    return d->callerID;
}


QString CallerIDObject::name() const
{
    return d->callerID.name;
}


void CallerIDObject::setName(const QString &name)
{
    if (d->callerID.name == name)
        return;

    d->callerID.name = name;
    emit changed();
}


QString CallerIDObject::number() const
{
    return d->callerID.number;
}


void CallerIDObject::setNumber(const QString &number)
{
    if (d->callerID.number == number)
        return;

    d->callerID.number = number;
    emit changed();
}


QString CallerIDObject::picture() const
{
    return d->callerID.picture;
}


void CallerIDObject::setPicture(const QString &picture)
{
    if (d->callerID.picture == picture)
        return;

    d->callerID.picture = picture;
    emit changed();
}
