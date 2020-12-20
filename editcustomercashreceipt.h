#ifndef EDITCUSTOMERCASHRECEIPT_H
#define EDITCUSTOMERCASHRECEIPT_H

#include <QWidget>
#include "dbcon.h"
#include "erp.h"
#include <QCompleter>
#include "sch.h"

namespace Ui {
class EditCustomerCashReceipt;
}

class EditCustomerCashReceipt : public QWidget
{
    Q_OBJECT

public:
    QString loginid;
    DbCon conn;
    ERP erp;
    SCH sch;
    explicit EditCustomerCashReceipt(QWidget *parent = 0);
    ~EditCustomerCashReceipt();


private slots:
    void autocompleter(QString sql, QLineEdit *txt1, QLineEdit *txt2);
    void onItemHighlighted(const QModelIndex &index);
    void editingFinished();


    void on_customername_textEdited(const QString &arg1);

    void on_AddButton_clicked();

    void on_removeRowButton_clicked();

    void on_recievedate_userDateChanged(const QDate &date);

    void on_SaveButton_clicked();



    void on_search_customername_textEdited(const QString &arg1);

    void on_search_btn_clicked();

    void on_EditButton_clicked();

    void on_DeleteButton_clicked();

private:
    Ui::EditCustomerCashReceipt *ui;
    enum columns{
        CUSTOMERNAME,CUSTOMERID,DESCRIPTION,AMOUNT
    };
    enum searchcolumns{
        VNO,VDATE,VAMOUNT
    };
    void checktotal();
    void clearForm();
    void loadEditForm();
    void loadSearchForm();
};

#endif // EDITCUSTOMERCASHRECEIPT_H
