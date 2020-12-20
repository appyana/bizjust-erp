#ifndef ADDCUSTOMER_H
#define ADDCUSTOMER_H

#include <QWidget>
#include "dbcon.h"
#include "erp.h"
#include <QCompleter>
#include "sch.h"
namespace Ui {
class AddCustomer;
}

class AddCustomer : public QWidget
{
    Q_OBJECT

public:
    QString loginid;
    DbCon conn;
    ERP erp;
    SCH sch;
    explicit AddCustomer(QWidget *parent = 0);
    ~AddCustomer();
    void loadcountries();

private slots:

    void autocompleter(QString sql, QLineEdit *txt1, QLineEdit *txt2);
    void onItemHighlighted(const QModelIndex &index);
    void editingFinished();

    void on_txt_group_textEdited();
    void on_txt_salesman_textEdited();
    void on_txt_termname_textEdited();
    void on_txt_vendormane_textEdited();//const QString &arg1

    void on_btn_save_clicked();
    bool isValid();
    void clearform();

    void on_btn_close_clicked();

private:
    Ui::AddCustomer *ui;
    QCompleter *groupCompleter;
};

#endif // ADDCUSTOMER_H
