#ifndef ADDVENDOROPENINGBALANCE_H
#define ADDVENDOROPENINGBALANCE_H

#include <QWidget>
#include "dbcon.h"
#include "erp.h"
#include <QCompleter>
#include "sch.h"

namespace Ui {
class AddVendorOpeningBalance;
}

class AddVendorOpeningBalance : public QWidget
{
    Q_OBJECT

public:
    QString loginid;
    DbCon conn;
    ERP erp;
    SCH sch;
    explicit AddVendorOpeningBalance(QWidget *parent = 0);
    ~AddVendorOpeningBalance();

private slots:
    void autocompleter(QString sql, QLineEdit *txt1, QLineEdit *txt2);
    void onItemHighlighted(const QModelIndex &index);
    void editingFinished();

    void on_vendorname_textEdited(const QString &arg1);

    void on_btn_add_clicked();

    void on_btn_delete_row_clicked();

    void on_btn_save_clicked();

    void on_debit_valueChanged(double arg1);

    void on_credit_valueChanged(double arg1);

private:
    Ui::AddVendorOpeningBalance *ui;
    enum columns{
        VENDORNAME,VENDORID,DEBIT,CREDIT
    };
    void loadform();
    void checktotal();
    void clearForm();
};

#endif // ADDVENDOROPENINGBALANCE_H
