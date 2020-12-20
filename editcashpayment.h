#ifndef EDITCASHPAYMENT_H
#define EDITCASHPAYMENT_H

#include <QWidget>
#include "dbcon.h"
#include "erp.h"
#include <QCompleter>
#include "sch.h"

namespace Ui {
class EditCashPayment;
}

class EditCashPayment : public QWidget
{
    Q_OBJECT

public:
    QString loginid;
    DbCon conn;
    ERP erp;
    SCH sch;
    explicit EditCashPayment(QWidget *parent = 0);
    ~EditCashPayment();

private slots:
    void autocompleter(QString sql, QLineEdit *txt1, QLineEdit *txt2);
    void onItemHighlighted(const QModelIndex &index);
    void editingFinished();

    void on_accountname_textEdited(const QString &arg1);

    void on_btn_add_clicked();

    void on_btn_delete_row_clicked();

    void on_btn_save_clicked();

    void on_btn_search_clicked();

    void on_btn_edit_clicked();

    void on_btn_delete_clicked();

private:
    Ui::EditCashPayment *ui;
    enum columns{
        ACCOUNTNAME,ACCOUNTID,DESCRIPTION,AMOUNT
    };
    enum searchcolumns{
        VNO,DATE
    };
    void loadform();
    void checktotal();
    void clearForm();
};

#endif // EDITCASHPAYMENT_H
