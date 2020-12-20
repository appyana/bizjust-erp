#ifndef EDITJOURNALVOUCHER_H
#define EDITJOURNALVOUCHER_H

#include <QWidget>
#include "dbcon.h"
#include "erp.h"
#include <QCompleter>
#include "sch.h"

namespace Ui {
class EditJournalVoucher;
}

class EditJournalVoucher : public QWidget
{
    Q_OBJECT

public:
    QString loginid;
    DbCon conn;
    ERP erp;
    SCH sch;
    explicit EditJournalVoucher(QWidget *parent = 0);
    ~EditJournalVoucher();

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

    void on_btn_search_clicked();

    void on_btn_edit_clicked();

    void on_btn_delete_clicked();

private:
    Ui::EditJournalVoucher *ui;
    enum columns{
        COMPANYTYPE,ACCOUNTNAME,ACCOUNTID,DESCRIPTION,DEBIT,CREDIT
    };
    enum searchcolumns{
        VNO,DATE
    };
    void loadform();
    void checktotal();
    void clearForm();
};

#endif // EDITJOURNALVOUCHER_H
