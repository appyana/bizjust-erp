#ifndef MANAGEVENDORGROUPS_H
#define MANAGEVENDORGROUPS_H

#include <QWidget>
#include "dbcon.h"
#include "erp.h"
#include <QCompleter>
#include "sch.h"

namespace Ui {
class ManageVendorGroups;
}

class ManageVendorGroups : public QWidget
{
    Q_OBJECT

public:
    QString loginid;
    DbCon conn;
    ERP erp;
    SCH sch;
    explicit ManageVendorGroups(QWidget *parent = 0);
    ~ManageVendorGroups();

private slots:
    void on_save_btn_clicked();

    void on_edit_btn_clicked();

    void on_clear_btn_clicked();

    void on_delete_btn_clicked();

private:
    Ui::ManageVendorGroups *ui;
    enum columns{
        ID,NAME
    };
    void loadtable();
    QString editid = "";
};

#endif // MANAGEVENDORGROUPS_H
