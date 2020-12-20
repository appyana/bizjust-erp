#ifndef ADDCUSTOMERCASHRECEIPT_H
#define ADDCUSTOMERCASHRECEIPT_H

#include <QWidget>
#include "dbcon.h"
#include "erp.h"
#include <QCompleter>
#include "sch.h"

namespace Ui {
class AddCustomerCashReceipt;
}

class AddCustomerCashReceipt : public QWidget
{
    Q_OBJECT

public:
    QString loginid;
    DbCon conn;
    ERP erp;
    SCH sch;
    explicit AddCustomerCashReceipt(QWidget *parent = 0);
    ~AddCustomerCashReceipt();


private slots:
    void autocompleter(QString sql, QLineEdit *txt1, QLineEdit *txt2);
    void onItemHighlighted(const QModelIndex &index);
    void editingFinished();


    void on_customername_textEdited(const QString &arg1);

    void on_AddButton_clicked();

    void on_removeRowButton_clicked();

    void on_recievedate_userDateChanged(const QDate &date);

    void on_SaveButton_clicked();

private:
    Ui::AddCustomerCashReceipt *ui;
    enum columns{
        CUSTOMERNAME,CUSTOMERID,DESCRIPTION,AMOUNT
    };
    void checktotal();
    void clearForm();
};

#endif // ADDCUSTOMERCASHRECEIPT_H
