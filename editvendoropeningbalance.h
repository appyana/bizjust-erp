#ifndef EDITVENDOROPENINGBALANCE_H
#define EDITVENDOROPENINGBALANCE_H

#include <QWidget>
#include "dbcon.h"
#include "erp.h"
#include <QCompleter>
#include "sch.h"

namespace Ui {
class EditVendorOpeningBalance;
}

class EditVendorOpeningBalance : public QWidget
{
    Q_OBJECT

public:
    QString loginid;
    DbCon conn;
    ERP erp;
    SCH sch;
    explicit EditVendorOpeningBalance(QWidget *parent = 0);
    ~EditVendorOpeningBalance();

private slots:
    void autocompleter(QString sql, QLineEdit *txt1, QLineEdit *txt2);
    void onItemHighlighted(const QModelIndex &index);
    void editingFinished();

    void on_vendorname_textEdited(const QString &arg1);

    void on_btn_save_clicked();

    void on_search_vendorname_textEdited(const QString &arg1);

    void on_btn_search_clicked();

    void on_btn_edit_clicked();

    void on_btn_delete_clicked();

    void on_debit_valueChanged(double arg1);

    void on_credit_valueChanged(double arg1);

private:
    Ui::EditVendorOpeningBalance *ui;
    enum columns{
        VENDORNAME,VENDORID,DEBIT,CREDIT
    };
    enum searchcolumns{
        VOUCHER,VENDOR
    };
    void loadform();
    void clearForm();
};

#endif // EDITVENDOROPENINGBALANCE_H
