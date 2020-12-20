#ifndef FINANCIALYEAR_H
#define FINANCIALYEAR_H

#include <QWidget>
#include "dbcon.h"
#include "erp.h"
#include <QCompleter>
#include "sch.h"

namespace Ui {
class FinancialYear;
}

class FinancialYear : public QWidget
{
    Q_OBJECT

public:
    QString loginid;
    DbCon conn;
    ERP erp;
    SCH sch;
    explicit FinancialYear(QWidget *parent = 0);
    ~FinancialYear();

private slots:
    void on_save_btn_clicked();

    void on_edit_btn_clicked();

    void on_clear_btn_clicked();

private:
    Ui::FinancialYear *ui;
    enum columns{
        ID,TITLE,STARTING,ENDING,DESCRIPTION,STATUS
    };
    void loadtable();
    QString editid = "";
};

#endif // FINANCIALYEAR_H
