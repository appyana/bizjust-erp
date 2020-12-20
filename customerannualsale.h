#ifndef CUSTOMERANNUALSALE_H
#define CUSTOMERANNUALSALE_H

#include <QWidget>
#include "dbcon.h"
#include "erp.h"
#include <QCompleter>
#include "sch.h"

namespace Ui {
class CustomerAnnualSale;
}

class CustomerAnnualSale : public QWidget
{
    Q_OBJECT

public:
    QString loginid;
    DbCon conn;
    ERP erp;
    SCH sch;
    explicit CustomerAnnualSale(QWidget *parent = 0);
    ~CustomerAnnualSale();

private slots:
    void autocompleter(QString sql, QLineEdit *txt1, QLineEdit *txt2);
    void onItemHighlighted(const QModelIndex &index);
    void editingFinished();
    void loadParams();

    void on_customername_textEdited(const QString &arg1);

    void on_nextButton_clicked();

private:
    Ui::CustomerAnnualSale *ui;
    QTimer *timer;
};

#endif // CUSTOMERANNUALSALE_H
