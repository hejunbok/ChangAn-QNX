#include "InsertNewAttributeDialog.h"

#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QVBoxLayout>

InsertNewAttributeDialog::InsertNewAttributeDialog(QWidget *parent)
    : QDialog(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    QWidget *main = new QWidget;
    QFormLayout *form = new QFormLayout(main);
    form->setMargin(0);

    m_keyEdit = new QLineEdit;
    form->addRow(tr("Name"), m_keyEdit);

    m_valueEdit = new QLineEdit;
    form->addRow(tr("Value"), m_valueEdit);

    m_encodingEdit = new QLineEdit;
    form->addRow(tr("Encoding"), m_encodingEdit);

    layout->addWidget(main);

    QDialogButtonBox *box = new QDialogButtonBox;
    box->setStandardButtons(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);
    connect(box, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(box, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(box);
}

QString InsertNewAttributeDialog::key() const
{
    return m_keyEdit->text();
}

QString InsertNewAttributeDialog::value() const
{
    return m_valueEdit->text();
}

QString InsertNewAttributeDialog::encoding() const
{
    return m_encodingEdit->text();
}
