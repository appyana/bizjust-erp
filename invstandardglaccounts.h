#ifndef INVSTANDARDGLACCOUNTS_H
#define INVSTANDARDGLACCOUNTS_H

#include <QWidget>
#include "dbcon.h"
#include "erp.h"
#include <QCompleter>
#include "sch.h"

namespace Ui {
class InvStandardGLAccounts;
}

class InvStandardGLAccounts : public QWidget
{
    Q_OBJECT

public:
    QString loginid;
    DbCon conn;
    ERP erp;
    SCH sch;
    explicit InvStandardGLAccounts(QWidget *parent = 0);
    ~InvStandardGLAccounts();

private slots:
    void autocompleter(QString sql, QLineEdit *txt1, QLineEdit *txt2);
    void onItemHighlighted(const QModelIndex &index);
    void editingFinished();
    void on_btn_save_clicked();

    void on_cashdescription_textEdited(const QString &arg1);

    void on_salesdescription_textEdited(const QString &arg1);

    void on_inventorydescription_textEdited(const QString &arg1);

    void on_lossdescription_textEdited(const QString &arg1);

    void on_costdescription_textEdited(const QString &arg1);

    void on_freightdescription_textEdited(const QString &arg1);

    void on_default_inventorydes_textEdited(const QString &arg1);

private:
    Ui::InvStandardGLAccounts *ui;
};

#endif // INVSTANDARDGLACCOUNTS_H
