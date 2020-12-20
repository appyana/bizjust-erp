#ifndef ADDGLACCOUNT_H
#define ADDGLACCOUNT_H

#include <QWidget>
#include "dbcon.h"
#include "erp.h"
#include <QCompleter>
#include "sch.h"
#include "comboboxes.h"

namespace Ui {
class AddGLAccount;
}

class AddGLAccount : public QWidget
{
    Q_OBJECT

public:
    QString loginid;
    DbCon conn;
    ERP erp;
    SCH sch;
    ComboBoxes cb;
    explicit AddGLAccount(QWidget *parent = 0);
    ~AddGLAccount();

private slots:
    void on_btn_save_clicked();

    void on_accounttypeid_currentIndexChanged(int index);

    void on_summaryaccountid_currentIndexChanged(int index);

private:
    Ui::AddGLAccount *ui;
    void loadForm();
    void loadaccountname(int index);
    QString checkvalidaccount(int name);
};

#endif // ADDGLACCOUNT_H
