#ifndef ADDCUSTOMEROPENINGBALANCE_H
#define ADDCUSTOMEROPENINGBALANCE_H

#include <QWidget>
#include "dbcon.h"
#include "erp.h"
#include <QCompleter>
#include "sch.h"

namespace Ui {
class AddCustomerOpeningBalance;
}

class AddCustomerOpeningBalance : public QWidget
{
    Q_OBJECT

public:
    QString loginid;
    DbCon conn;
    ERP erp;
    SCH sch;
    explicit AddCustomerOpeningBalance(QWidget *parent = 0);
    ~AddCustomerOpeningBalance();

private slots:
    void autocompleter(QString sql, QLineEdit *txt1, QLineEdit *txt2);
    void onItemHighlighted(const QModelIndex &index);
    void editingFinished();

    void on_customername_textEdited(const QString &arg1);

    void on_btn_add_clicked();

    void on_btn_delete_row_clicked();
    
    void on_btn_save_clicked();
    
private:
    Ui::AddCustomerOpeningBalance *ui;
    enum columns{
        CUSTOMERNAME,CUSTOMERID,AMOUNT
    };
    void loadform();
    void checktotal();
    void clearForm();
};

#endif // ADDCUSTOMEROPENINGBALANCE_H
