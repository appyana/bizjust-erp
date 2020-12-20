#ifndef MANAGEUSERS_H
#define MANAGEUSERS_H

#include <QWidget>
#include "dbcon.h"
#include "erp.h"
#include <QCompleter>
#include "sch.h"

namespace Ui {
class ManageUsers;
}

class ManageUsers : public QWidget
{
    Q_OBJECT

public:
    QString loginid;
    DbCon conn;
    ERP erp;
    SCH sch;
    explicit ManageUsers(QWidget *parent = 0);
    ~ManageUsers();

private slots:
    void on_btn_add_clicked();

    void on_btn_edit_clicked();

    void on_btn_rights_clicked();

    void on_btn_back_clicked();

    void on_btn_save_clicked();

    void on_module_currentIndexChanged(int index);

    void chk_clicked(bool arg1);

    void on_back_btn_clicked();

    void on_btn_delete_clicked();

private:
    Ui::ManageUsers *ui;
    enum{
        ID,TITLE,ACTION
    };
    void loadusers();
    QString editid;
};

#endif // MANAGEUSERS_H
