#ifndef ADDPURCHASERETURN_H
#define ADDPURCHASERETURN_H

#include <QWidget>
#include "dbcon.h"
#include "erp.h"
#include <QCompleter>
#include "sch.h"

namespace Ui {
class AddPurchaseReturn;
}

class AddPurchaseReturn : public QWidget
{
    Q_OBJECT

public:
    QString loginid;
    DbCon conn;
    ERP erp;
    SCH sch;
    explicit AddPurchaseReturn(QWidget *parent = 0);
    ~AddPurchaseReturn();

private slots:
    void autocompleter(QString sql, QLineEdit *txt1, QLineEdit *txt2);
    void onItemHighlighted(const QModelIndex &index);
    void editingFinished();

    void on_vendorname_textEdited(const QString &arg1);

    void on_itemname_textEdited(const QString &arg1);

    void on_itemid_textChanged(const QString &arg1);

    void on_btn_add_clicked();

    void on_btn_delete_row_clicked();

    void on_btn_save_clicked();

    void on_qty_valueChanged(int arg1);

    void on_price_valueChanged(double arg1);

private:
    Ui::AddPurchaseReturn *ui;
    enum columns{
        ITEM, ITEMID, LOCATION, LOCATIONID, QTY,UNITNAME,UNITID, PRICE, AMOUNT
    };
    void checkamt();
    void checktotal();
    bool isValid();
    void clearform();
};

#endif // ADDPURCHASERETURN_H
