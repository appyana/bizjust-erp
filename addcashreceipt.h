#ifndef ADDCASHRECEIPT_H
#define ADDCASHRECEIPT_H

#include <QWidget>
#include "dbcon.h"
#include "erp.h"
#include <QCompleter>
#include "sch.h"

namespace Ui {
class AddCashReceipt;
}

class AddCashReceipt : public QWidget
{
    Q_OBJECT

public:
    QString loginid;
    DbCon conn;
    ERP erp;
    SCH sch;
    explicit AddCashReceipt(QWidget *parent = 0);
    ~AddCashReceipt();

private slots:
    void autocompleter(QString sql, QLineEdit *txt1, QLineEdit *txt2);
    void onItemHighlighted(const QModelIndex &index);
    void editingFinished();

    void on_accountname_textEdited(const QString &arg1);

    void on_btn_add_clicked();

    void on_btn_delete_row_clicked();

    void on_btn_save_clicked();

private:
    Ui::AddCashReceipt *ui;
    enum columns{
        ACCOUNTNAME,ACCOUNTID,DESCRIPTION,AMOUNT
    };
    void loadform();
    void checktotal();
    void clearForm();
};

#endif // ADDCASHRECEIPT_H
