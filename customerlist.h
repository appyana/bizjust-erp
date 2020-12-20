#ifndef CUSTOMERLIST_H
#define CUSTOMERLIST_H

#include <QWidget>
#include <QtCore>
#include <QtGui>
#include "erp.h"
#include <QCompleter>
#include "sch.h"

namespace Ui {
class CustomerList;
}

class CustomerList : public QWidget
{
    Q_OBJECT

public:
    QString loginid;
    ERP erp;
    SCH sch;
    explicit CustomerList(QWidget *parent = 0);
    ~CustomerList();

private slots:
    void loadCustomers();

private:
    Ui::CustomerList *ui;
};

#endif // CUSTOMERLIST_H
