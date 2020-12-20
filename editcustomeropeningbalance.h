#ifndef EDITCUSTOMEROPENINGBALANCE_H
#define EDITCUSTOMEROPENINGBALANCE_H

#include <QWidget>
#include "dbcon.h"
#include "erp.h"
#include <QCompleter>
#include "sch.h"

namespace Ui {
class EditCustomerOpeningBalance;
}

class EditCustomerOpeningBalance : public QWidget
{
    Q_OBJECT

public:
    QString loginid;
    DbCon conn;
    ERP erp;
    SCH sch;
    explicit EditCustomerOpeningBalance(QWidget *parent = 0);
    ~EditCustomerOpeningBalance();

private slots:
    void autocompleter(QString sql, QLineEdit *txt1, QLineEdit *txt2);
    void onItemHighlighted(const QModelIndex &index);
    void editingFinished();

    void on_customername_textEdited(const QString &arg1);

    void on_btn_save_clicked();

    void on_search_customername_textEdited(const QString &arg1);

    void on_btn_search_clicked();

    void on_btn_edit_clicked();

    void on_btn_delete_clicked();

private:
    Ui::EditCustomerOpeningBalance *ui;
    enum columns{
        CUSTOMERNAME,CUSTOMERID,AMOUNT
    };
    enum searchcolumns{
        VOUCHER,CUSTOMER
    };
    void loadform();
    void clearForm();
};

#endif // EDITCUSTOMEROPENINGBALANCE_H
