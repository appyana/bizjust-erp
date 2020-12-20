#ifndef MANAGEUNITS_H
#define MANAGEUNITS_H

#include <QWidget>
#include "dbcon.h"
#include "erp.h"
#include <QCompleter>
#include "sch.h"

namespace Ui {
class ManageUnits;
}

class ManageUnits : public QWidget
{
    Q_OBJECT

public:
    QString loginid;
    DbCon conn;
    ERP erp;
    SCH sch;
    explicit ManageUnits(QWidget *parent = 0);
    ~ManageUnits();

private slots:
    void on_save_btn_clicked();

    void on_edit_btn_clicked();

    void on_clear_btn_clicked();

    void on_delete_btn_clicked();

private:
    Ui::ManageUnits *ui;
    enum columns{
        ID,NAME
    };
    void loadtable();
    QString editid = "";
};

#endif // MANAGEUNITS_H
