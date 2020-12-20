#ifndef EDITPURCHASEINVOICE_H
#define EDITPURCHASEINVOICE_H

#include <QWidget>
#include "dbcon.h"
#include "erp.h"
#include <QCompleter>
#include "sch.h"

namespace Ui {
class EditPurchaseInvoice;
}

class EditPurchaseInvoice : public QWidget
{
    Q_OBJECT

public:
    QString loginid;
    DbCon conn;
    ERP erp;
    SCH sch;
    explicit EditPurchaseInvoice(QWidget *parent = 0);
    ~EditPurchaseInvoice();

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

    void loadForm();

    void on_btn_search_clicked();

    void on_btn_edit_clicked();

    void on_btn_delete_clicked();

    void on_btn_back_clicked();

    void on_search_vendorname_textEdited(const QString &arg1);

private:
    Ui::EditPurchaseInvoice *ui;
    enum columns{
        ITEM, ITEMID, LOCATION, LOCATIONID, QTY,UNITNAME,UNITID, PRICE, AMOUNT
    };
    enum searchcolumns{
        PINUMBER, VENDORNAME, ENTRYDATE, SEARCHAMOUNT
    };
    QString editvno;
    void checkamt();
    void checktotal();
    bool isValid();
    void clearform();
    QTimer *timer;
};

#endif // EDITPURCHASEINVOICE_H
