#ifndef SQLTABLEMODEL_P_H
#define SQLTABLEMODEL_P_H

namespace QnxCar {

/**
 * Private data of SqlTableModel
 *
 * "Hidden" behind a d-pointer (PIMPL idiom) for enabling binary compatibility
 * if data members need to added, removed or changed
 */
class SqlTableModel::Private
{
    // allow only model base class to direcly access "db"
    friend class SqlTableModel;

public:
    virtual ~Private() {}

private:
    QSqlDatabase db;
};

}

#ifndef D_PTR
#define D_PTR d_func<Private>()
#endif

#endif // SQLTABLEMODEL_P_H
