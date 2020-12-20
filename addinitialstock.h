#ifndef ADDINITIALSTOCK_H
#define ADDINITIALSTOCK_H

#include <QWidget>
#include "dbcon.h"
#include "erp.h"
#include <QCompleter>
#include "sch.h"
#include "comboboxes.h"

namespace Ui {
class AddInitialStock;
}

class AddInitialStock : public QWidget
{
    Q_OBJECT

public:
    QString loginid;
    DbCon conn;
    ERP erp;
    SCH sch;
    ComboBoxes cb;
    explicit AddInitialStock(QWidget *parent = 0);
    ~AddInitialStock();

private slots:
    void autocompleter(QString sql, QLineEdit *txt1, QLineEdit *txt2);
    void onItemHighlighted(const QModelIndex &index);
    void editingFinished();

    void on_itemname_textEdited(const QString &arg1);

    void on_btn_add_clicked();

    void on_itemid_textChanged(const QString &arg1);

    void on_qty_valueChanged(int arg1);

    void on_price_valueChanged(double arg1);

    void on_btn_delete_row_clicked();

    void on_btn_save_clicked();

private:
    Ui::AddInitialStock *ui;
    enum columns{
        ITEMNAME,ITEMID,LOCATIONNAME,LOCATIONID,UNITNAME,UNITID,QUANTITY,PRICE,AMOUNT
    };
    void loadform();
    void checktotal();
    void clearForm();
};

#endif // ADDINITIALSTOCK_H
