#ifndef MANAGESALESCATEGORIES_H
#define MANAGESALESCATEGORIES_H

#include <QWidget>
#include "dbcon.h"
#include "erp.h"
#include <QCompleter>
#include "sch.h"

namespace Ui {
class ManageSalesCategories;
}

class ManageSalesCategories : public QWidget
{
    Q_OBJECT

public:
    QString loginid;
    DbCon conn;
    ERP erp;
    SCH sch;
    explicit ManageSalesCategories(QWidget *parent = 0);
    ~ManageSalesCategories();

private slots:
    void on_save_btn_clicked();

    void on_edit_btn_clicked();

    void on_clear_btn_clicked();

    void on_delete_btn_clicked();

private:
    Ui::ManageSalesCategories *ui;
    enum columns{
        ID,NAME
    };
    void loadtable();
    QString editid = "";
};

#endif // MANAGESALESCATEGORIES_H
