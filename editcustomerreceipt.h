#ifndef EDITCUSTOMERRECEIPT_H
#define EDITCUSTOMERRECEIPT_H

#include <QWidget>
#include "dbcon.h"
#include "erp.h"
#include <QCompleter>
#include "sch.h"

namespace Ui {
class EditCustomerReceipt;
}

class EditCustomerReceipt : public QWidget
{
    Q_OBJECT

public:
    QString loginid;
    DbCon conn;
    ERP erp;
    SCH sch;
    explicit EditCustomerReceipt(QWidget *parent = 0);
    ~EditCustomerReceipt();


private slots:
    void autocompleter(QString sql, QLineEdit *txt1, QLineEdit *txt2);
    void onItemHighlighted(const QModelIndex &index);
    void editingFinished();

    void on_customername_textEdited(const QString &arg1);

    void on_paymeth_currentIndexChanged(int index);

    void on_depositdescription_textEdited(const QString &arg1);

    void on_customerid_textChanged(const QString &arg1);

    void on_SaveButton_clicked();


    void on_search_customername_textEdited(const QString &arg1);

    void on_search_btn_clicked();

    void on_EditButton_clicked();

    void on_DeleteButton_clicked();

private:
    Ui::EditCustomerReceipt *ui;
    enum columns{
        VOUCHER,VDATE,CUSTOMER,AMOUNT
    };
    void loadpaymenth();
    bool isValid();
    void clearform();
    void loadsearchtable();
};

#endif // EDITCUSTOMERRECEIPT_H
