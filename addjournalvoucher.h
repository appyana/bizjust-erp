#ifndef ADDJOURNALVOUCHER_H
#define ADDJOURNALVOUCHER_H

#include <QWidget>
#include "dbcon.h"
#include "erp.h"
#include <QCompleter>
#include "sch.h"

namespace Ui {
class AddJournalVoucher;
}

class AddJournalVoucher : public QWidget
{
    Q_OBJECT

public:
    QString loginid;
    DbCon conn;
    ERP erp;
    SCH sch;
    explicit AddJournalVoucher(QWidget *parent = 0);
    ~AddJournalVoucher();

private slots:
    void autocompleter(QString sql, QLineEdit *txt1, QLineEdit *txt2);
    void onItemHighlighted(const QModelIndex &index);
    void editingFinished();

    void on_accountname_textEdited(const QString &arg1);

    void on_btn_add_clicked();

    void on_btn_delete_row_clicked();

    void on_btn_save_clicked();

    void on_companytype_currentIndexChanged(int index);

    void on_debit_valueChanged(double arg1);

    void on_credit_valueChanged(double arg1);

private:
    Ui::AddJournalVoucher *ui;
    enum columns{
        COMPANYTYPE,ACCOUNTNAME,ACCOUNTID,DESCRIPTION,DEBIT,CREDIT
    };
    void loadform();
    void checktotal();
    void clearForm();
};

#endif // ADDJOURNALVOUCHER_H
