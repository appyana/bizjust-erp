#ifndef EDITSALESRETURN_H
#define EDITSALESRETURN_H

#include <QWidget>
#include "dbcon.h"
#include "erp.h"
#include <QCompleter>
#include "sch.h"

namespace Ui {
class EditSalesReturn;
}

class EditSalesReturn : public QWidget
{
    Q_OBJECT

public:
    QString loginid;
    DbCon conn;
    ERP erp;
    SCH sch;
    explicit EditSalesReturn(QWidget *parent = 0);
    ~EditSalesReturn();


private slots:
    void autocompleter(QString sql, QLineEdit *txt1, QLineEdit *txt2);
    void onItemHighlighted(const QModelIndex &index);
    void editingFinished();

    void on_txt_customername_textEdited(const QString &arg1);

    void on_txt_itemname_textEdited(const QString &arg1);

    void on_txt_qty_valueChanged();

    void on_txt_price_valueChanged();

    void on_txt_amt_valueChanged();

    void on_btn_add_clicked();
    void on_btn1_clicked();


    void on_txt_discount_valueChanged();

    void on_txt_freight_valueChanged();

    void on_txt_customerid_textChanged();

    void on_dateEntrydate_userDateChanged();

    void on_txt_totalamt_valueChanged();

    void on_txt_totNetAmount_valueChanged();

    void on_comboSales_type_currentIndexChanged();//int index

    void on_btn_save_clicked();

    void on_txt_itemid_textChanged(const QString &arg1);


    void on_btn_next_clicked();

    void on_pushButton_Edit_clicked();

    void on_pushButton_Delete_clicked();

    void on_btn_back_clicked();

    void on_search_customername_textEdited(const QString &arg1);

private:
    Ui::EditSalesReturn *ui;
    enum columns{
        ITEM, ITEMID, LOCATION, LOCATIONID, QTY,UNITNAME,UNITID, PRICE, AMOUNT, BUTTON
    };
    enum searchcolumns{
        RETNO,RETAMOUNT,RETDATE,CUSTOMER
    };
    void checkamt();
    void checktotal();
    bool isValid();
    void clearform();
    void loadSearchForm();
    void loadEditForm();
};

#endif // EDITSALESRETURN_H
