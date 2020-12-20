#ifndef ADDGLOPENINGBALANCE_H
#define ADDGLOPENINGBALANCE_H

#include <QWidget>
#include "dbcon.h"
#include "erp.h"
#include <QCompleter>
#include "sch.h"

namespace Ui {
class AddGLOpeningBalance;
}

class AddGLOpeningBalance : public QWidget
{
    Q_OBJECT

public:
    QString loginid;
    DbCon conn;
    ERP erp;
    SCH sch;
    explicit AddGLOpeningBalance(QWidget *parent = 0);
    ~AddGLOpeningBalance();

private slots:
    void autocompleter(QString sql, QLineEdit *txt1, QLineEdit *txt2);
    void onItemHighlighted(const QModelIndex &index);
    void editingFinished();

    void on_accountname_textEdited(const QString &arg1);

    void on_btn_add_clicked();

    void on_btn_delete_row_clicked();

    void on_btn_save_clicked();

    void on_debit_valueChanged(double arg1);

    void on_credit_valueChanged(double arg1);

private:
    Ui::AddGLOpeningBalance *ui;
    enum columns{
        ACCOUNTNAME,ACCOUNTID,DESCRIPTION,DEBIT,CREDIT
    };
    void loadform();
    void checktotal();
    void clearForm();
};

#endif // ADDGLOPENINGBALANCE_H
