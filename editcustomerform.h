#ifndef EDITCUSTOMERFORM_H
#define EDITCUSTOMERFORM_H

#include <QWidget>
#include <QtCore>
#include <QtGui>
#include "dbcon.h"
#include "erp.h"
#include <QCompleter>
#include "sch.h"

namespace Ui {
class EditCustomerForm;
}

class EditCustomerForm : public QWidget
{
    Q_OBJECT

public:
    QString customerid;
    QString loginid;
    DbCon conn;
    ERP erp;
    SCH sch;
    explicit EditCustomerForm(QWidget *parent = 0);
    ~EditCustomerForm();
    void loadCustomerData();

private slots:
    void autocompleter(QString sql, QLineEdit *txt1, QLineEdit *txt2);
    void onItemHighlighted(const QModelIndex &index);
    void editingFinished();

    void on_txt_group_textEdited();
    void on_txt_salesman_textEdited();
    void on_txt_termname_textEdited();
    void on_txt_vendormane_textEdited();
    void on_txt_customername_textEdited();

    void on_btn_save_clicked();
    bool isValid();
    void clearform();


    void on_btn_close_clicked();
    void on_btn_delete_clicked();
    void on_btn_activate_clicked();


    void on_txt_customer_id_textChanged();

private:
    Ui::EditCustomerForm *ui;
};

#endif // EDITCUSTOMERFORM_H
