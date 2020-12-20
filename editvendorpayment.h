#ifndef EDITVENDORPAYMENT_H
#define EDITVENDORPAYMENT_H

#include <QWidget>
#include "dbcon.h"
#include "erp.h"
#include <QCompleter>
#include "sch.h"

namespace Ui {
class EditVendorPayment;
}

class EditVendorPayment : public QWidget
{
    Q_OBJECT

public:
    QString loginid;
    DbCon conn;
    ERP erp;
    SCH sch;
    explicit EditVendorPayment(QWidget *parent = 0);
    ~EditVendorPayment();

private slots:
    void autocompleter(QString sql, QLineEdit *txt1, QLineEdit *txt2);
    void onItemHighlighted(const QModelIndex &index);
    void editingFinished();

    void on_btn_save_clicked();

    void on_vendorname_textEdited(const QString &arg1);

    void on_search_vendorname_textEdited(const QString &arg1);

    void on_btn_search_clicked();

    void on_btn_edit_clicked();

    void on_btn_delete_clicked();

    void on_btn_back_clicked();

private:
    Ui::EditVendorPayment *ui;
    enum columns{
        VNO,DATE,VENDOR,AMOUNT
    };
};

#endif // EDITVENDORPAYMENT_H
