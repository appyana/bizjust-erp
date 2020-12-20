#ifndef MANAGEITEMBRANDS_H
#define MANAGEITEMBRANDS_H

#include <QWidget>
#include "dbcon.h"
#include "erp.h"
#include <QCompleter>
#include "sch.h"

namespace Ui {
class ManageItemBrands;
}

class ManageItemBrands : public QWidget
{
    Q_OBJECT

public:
    QString loginid;
    DbCon conn;
    ERP erp;
    SCH sch;
    explicit ManageItemBrands(QWidget *parent = 0);
    ~ManageItemBrands();

private slots:
    void on_save_btn_clicked();

    void on_edit_btn_clicked();

    void on_clear_btn_clicked();

    void on_delete_btn_clicked();

private:
    Ui::ManageItemBrands *ui;
    enum columns{
        ID,NAME
    };
    void loadtable();
    QString editid = "";
};

#endif // MANAGEITEMBRANDS_H
