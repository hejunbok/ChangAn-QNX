#ifndef SQLQUERYMODEL_P_H
#define SQLQUERYMODEL_P_H

namespace QnxCar {

/**
 * Private data of SqlQueryModel
 *
 * "Hidden" behind a d-pointer (PIMPL idiom) for enabling binary compatibility
 * if data members need to added, removed or changed
 */
class SqlQueryModel::Private
{
    // allow only model base class to direcly access "db"
    friend class SqlQueryModel;

public:
    virtual ~Private() {}

private:
    QSqlDatabase db;
};

}

#ifndef D_PTR
#define D_PTR d_func<Private>()
#endif

#endif // SQLQUERYMODEL_P_H
