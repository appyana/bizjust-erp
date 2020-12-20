#ifndef CUSTOMERRECEIPT_H
#define CUSTOMERRECEIPT_H

#include <QWidget>
#include "dbcon.h"
#include "erp.h"
#include <QCompleter>
#include "sch.h"

namespace Ui {
class CustomerReceipt;
}

class CustomerReceipt : public QWidget
{
    Q_OBJECT

public:
    QString loginid;
    DbCon conn;
    ERP erp;
    SCH sch;
    explicit CustomerReceipt(QWidget *parent = 0);
    ~CustomerReceipt();


private slots:
    void autocompleter(QString sql, QLineEdit *txt1, QLineEdit *txt2);
    void onItemHighlighted(const QModelIndex &index);
    void editingFinished();



    void on_customername_textEdited(const QString &arg1);

    void on_paymeth_currentIndexChanged(int index);

    void on_depositdescription_textEdited(const QString &arg1);

    void on_customerid_textChanged(const QString &arg1);

    void on_pushButton_clicked();

private:
    Ui::CustomerReceipt *ui;
    void loadpaymenth();
    bool isValid();
    void clearform();
};

#endif // CUSTOMERRECEIPT_H
