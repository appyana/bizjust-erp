#ifndef APSTANDARDGLACCOUNTS_H
#define APSTANDARDGLACCOUNTS_H

#include <QWidget>
#include "dbcon.h"
#include "erp.h"
#include <QCompleter>
#include "sch.h"

namespace Ui {
class APStandardGLAccounts;
}

class APStandardGLAccounts : public QWidget
{
    Q_OBJECT

public:
    QString loginid;
    DbCon conn;
    ERP erp;
    SCH sch;
    explicit APStandardGLAccounts(QWidget *parent = 0);
    ~APStandardGLAccounts();

private slots:
    void autocompleter(QString sql, QLineEdit *txt1, QLineEdit *txt2);
    void onItemHighlighted(const QModelIndex &index);
    void editingFinished();
    void on_btn_save_clicked();

    void on_payabledescription_textEdited(const QString &arg1);

    void on_discountdescription_textEdited(const QString &arg1);

    void on_glcashiddescription_textEdited(const QString &arg1);

    void on_glcciddescription_textEdited(const QString &arg1);

    void on_glcommissioniddescription_textEdited(const QString &arg1);

    void on_txt_freight_textEdited(const QString &arg1);

    void on_txt_dfreight_textEdited(const QString &arg1);

    void on_purchasedescription_textEdited(const QString &arg1);

    void on_txt_inventory_textEdited(const QString &arg1);

    void on_txt_tax_textEdited(const QString &arg1);

    void on_txt_labour_textEdited(const QString &arg1);

    void on_txt_duty_textEdited(const QString &arg1);

    void on_txt_bank_textEdited(const QString &arg1);

    void on_txt_roff_textEdited(const QString &arg1);

    void on_txt_bcs_textEdited(const QString &arg1);

    void on_txt_empc_textEdited(const QString &arg1);

private:
    Ui::APStandardGLAccounts *ui;
};

#endif // APSTANDARDGLACCOUNTS_H
