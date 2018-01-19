#ifndef INSERTNEWATTRIBUTEDIALOG_H
#define INSERTNEWATTRIBUTEDIALOG_H

#include <QDialog>

class QLineEdit;

class InsertNewAttributeDialog : public QDialog
{
    Q_OBJECT
public:
    explicit InsertNewAttributeDialog(QWidget *parent = 0);

    QString key() const;
    QString value() const;
    QString encoding() const;

private:
    QLineEdit *m_keyEdit;
    QLineEdit *m_valueEdit;
    QLineEdit *m_encodingEdit;
};

#endif // INSERTNEWATTRIBUTEDIALOG_H
