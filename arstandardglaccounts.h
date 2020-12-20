#ifndef ARSTANDARDGLACCOUNTS_H
#define ARSTANDARDGLACCOUNTS_H

#include <QWidget>
#include "dbcon.h"
#include "erp.h"
#include <QCompleter>
#include "sch.h"

namespace Ui {
class ARStandardGLAccounts;
}

class ARStandardGLAccounts : public QWidget
{
    Q_OBJECT

public:
    QString loginid;
    DbCon conn;
    ERP erp;
    SCH sch;
    explicit ARStandardGLAccounts(QWidget *parent = 0);
    ~ARStandardGLAccounts();

private slots:
    void autocompleter(QString sql, QLineEdit *txt1, QLineEdit *txt2);
    void onItemHighlighted(const QModelIndex &index);
    void editingFinished();

    void on_cashAccountName_textEdited(const QString &arg1);

    void on_suspectAccountName_textEdited(const QString &arg1);

    void on_discountGivenAccountName_textEdited(const QString &arg1);

    void on_costOfGoodsAccountName_textEdited(const QString &arg1);

    void on_inventoryAccountName_textEdited(const QString &arg1);

    void on_shippingLiabilityAccountName_textEdited(const QString &arg1);

    void on_receivableAccountName_textEdited(const QString &arg1);

    void on_salesReturnAccountName_textEdited(const QString &arg1);

    void on_customerDepositAccountName_textEdited(const QString &arg1);

    void on_salesIncomeAccountName_textEdited(const QString &arg1);

    void on_freightAccountName_textEdited(const QString &arg1);

    void on_bankChargesAccountName_textEdited(const QString &arg1);

    void on_writeOffAccountName_textEdited(const QString &arg1);

    void on_btn_save_clicked();

private:
    Ui::ARStandardGLAccounts *ui;
};

#endif // ARSTANDARDGLACCOUNTS_H
