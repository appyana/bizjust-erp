#ifndef ADDCASHPAYMENT_H
#define ADDCASHPAYMENT_H

#include <QWidget>
#include "dbcon.h"
#include "erp.h"
#include <QCompleter>
#include "sch.h"

namespace Ui {
class AddCashPayment;
}

class AddCashPayment : public QWidget
{
    Q_OBJECT

public:
    QString loginid;
    DbCon conn;
    ERP erp;
    SCH sch;
    explicit AddCashPayment(QWidget *parent = 0);
    ~AddCashPayment();

private slots:
    void autocompleter(QString sql, QLineEdit *txt1, QLineEdit *txt2);
    void onItemHighlighted(const QModelIndex &index);
    void editingFinished();

    void on_accountname_textEdited(const QString &arg1);

    void on_btn_add_clicked();

    void on_btn_delete_row_clicked();

    void on_btn_save_clicked();

private:
    Ui::AddCashPayment *ui;
    enum columns{
        ACCOUNTNAME,ACCOUNTID,DESCRIPTION,AMOUNT
    };
    void loadform();
    void checktotal();
    void clearForm();
};

#endif // ADDCASHPAYMENT_H
