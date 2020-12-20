#ifndef MANAGEITEMSUBCATEGORY_H
#define MANAGEITEMSUBCATEGORY_H

#include <QWidget>
#include "dbcon.h"
#include "erp.h"
#include <QCompleter>
#include "sch.h"
#include "comboboxes.h"

namespace Ui {
class ManageItemSubCategory;
}

class ManageItemSubCategory : public QWidget
{
    Q_OBJECT

public:
    QString loginid;
    DbCon conn;
    ERP erp;
    SCH sch;
    ComboBoxes cb;
    explicit ManageItemSubCategory(QWidget *parent = 0);
    ~ManageItemSubCategory();

private slots:
    void on_save_btn_clicked();

    void on_edit_btn_clicked();

    void on_clear_btn_clicked();

    void on_delete_btn_clicked();

private:
    Ui::ManageItemSubCategory *ui;
    enum columns{
        ID,CATEGORY,NAME
    };
    void loadtable();
    QString editid = "";
};

#endif // MANAGEITEMSUBCATEGORY_H
