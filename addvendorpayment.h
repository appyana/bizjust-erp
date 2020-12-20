#ifndef ADDVENDORPAYMENT_H
#define ADDVENDORPAYMENT_H

#include <QWidget>
#include "dbcon.h"
#include "erp.h"
#include <QCompleter>
#include "sch.h"

namespace Ui {
class AddVendorPayment;
}

class AddVendorPayment : public QWidget
{
    Q_OBJECT

public:
    QString loginid;
    DbCon conn;
    ERP erp;
    SCH sch;
    explicit AddVendorPayment(QWidget *parent = 0);
    ~AddVendorPayment();

private slots:
    void autocompleter(QString sql, QLineEdit *txt1, QLineEdit *txt2);
    void onItemHighlighted(const QModelIndex &index);
    void editingFinished();

    void on_btn_save_clicked();

    void on_vendorname_textEdited(const QString &arg1);

private:
    Ui::AddVendorPayment *ui;
};

#endif // ADDVENDORPAYMENT_H
