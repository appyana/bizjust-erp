#ifndef EDITGLACCOUNT_H
#define EDITGLACCOUNT_H

#include <QWidget>
#include "dbcon.h"
#include "erp.h"
#include <QCompleter>
#include "sch.h"
#include "comboboxes.h"

namespace Ui {
class EditGLAccount;
}

class EditGLAccount : public QWidget
{
    Q_OBJECT

public:
    QString loginid;
    DbCon conn;
    ERP erp;
    SCH sch;
    ComboBoxes cb;
    explicit EditGLAccount(QWidget *parent = 0);
    ~EditGLAccount();

private slots:
    void autocompleter(QString sql, QLineEdit *txt1, QLineEdit *txt2);
    void onItemHighlighted(const QModelIndex &index);
    void editingFinished();

    void on_btn_save_clicked();

    void on_accounttypeid_currentIndexChanged(int index);

    void on_summaryaccountid_currentIndexChanged(int index);

    void on_accountname_textEdited(const QString &arg1);

    void on_accountid_textChanged(const QString &arg1);

    void on_btn_delete_clicked();

private:
    Ui::EditGLAccount *ui;
    void loadForm();
    void loadaccountname(int index);
    QString checkvalidaccount(int name);
};

#endif // EDITGLACCOUNT_H
