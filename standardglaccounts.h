#ifndef STANDARDGLACCOUNTS_H
#define STANDARDGLACCOUNTS_H

#include <QWidget>
#include "dbcon.h"
#include "erp.h"
#include <QCompleter>
#include "sch.h"

namespace Ui {
class StandardGLAccounts;
}

class StandardGLAccounts : public QWidget
{
    Q_OBJECT

public:
    QString loginid;
    DbCon conn;
    ERP erp;
    SCH sch;
    explicit StandardGLAccounts(QWidget *parent = 0);
    ~StandardGLAccounts();

private slots:
    void autocompleter(QString sql, QLineEdit *txt1, QLineEdit *txt2);
    void onItemHighlighted(const QModelIndex &index);
    void editingFinished();
    void on_btn_save_clicked();

    void on_earningdescription_textEdited(const QString &arg1);

    void on_bankacctdesc_textEdited(const QString &arg1);

    void on_purchiedescription_textEdited(const QString &arg1);

private:
    Ui::StandardGLAccounts *ui;
};

#endif // STANDARDGLACCOUNTS_H
