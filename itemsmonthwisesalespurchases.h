#ifndef ITEMSMONTHWISESALESPURCHASES_H
#define ITEMSMONTHWISESALESPURCHASES_H

#include <QWidget>
#include "dbcon.h"
#include "erp.h"
#include <QCompleter>
#include "sch.h"
#include <QtCore>
#include <QtGui>

namespace Ui {
class ItemsMonthWiseSalesPurchases;
}

class ItemsMonthWiseSalesPurchases : public QWidget
{
    Q_OBJECT

public:
    QString loginid;
    DbCon conn;
    ERP erp;
    SCH sch;
    explicit ItemsMonthWiseSalesPurchases(QWidget *parent = 0);
    ~ItemsMonthWiseSalesPurchases();

private slots:
    void autocompleter(QString sql, QLineEdit *txt1, QLineEdit *txt2);
    void onItemHighlighted(const QModelIndex &index);
    void editingFinished();

    void on_itemname_textEdited(const QString &arg1);

    void on_btn_next_clicked();

    void on_previous_year_btn_clicked();

    void on_next_year_btn_clicked();

private:
    Ui::ItemsMonthWiseSalesPurchases *ui;
    void loarForm();
    void viewDetail();
    QString foryear = QDate::currentDate().toString("yyyy");
    QString itemid="";
};

#endif // ITEMSMONTHWISESALESPURCHASES_H
