#ifndef MANAGELOCATIONS_H
#define MANAGELOCATIONS_H

#include <QWidget>
#include "dbcon.h"
#include "erp.h"
#include <QCompleter>
#include "sch.h"

namespace Ui {
class ManageLocations;
}

class ManageLocations : public QWidget
{
    Q_OBJECT

public:
    QString loginid;
    DbCon conn;
    ERP erp;
    SCH sch;
    explicit ManageLocations(QWidget *parent = 0);
    ~ManageLocations();

private slots:
    void on_save_btn_clicked();

    void on_edit_btn_clicked();

    void on_clear_btn_clicked();

    void on_delete_btn_clicked();

private:
    Ui::ManageLocations *ui;
    enum columns{
        ID,NAME
    };
    void loadtable();
    QString editid = "";
};

#endif // MANAGELOCATIONS_H
