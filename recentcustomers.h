#ifndef RECENTCUSTOMERS_H
#define RECENTCUSTOMERS_H

#include <QWidget>
#include "dbcon.h"
#include "erp.h"
#include <QCompleter>
#include "sch.h"

namespace Ui {
class RecentCustomers;
}

class RecentCustomers : public QWidget
{
    Q_OBJECT

public:
    QString loginid;
    DbCon conn;
    ERP erp;
    SCH sch;
    explicit RecentCustomers(QWidget *parent = 0);
    ~RecentCustomers();

private slots:
    void on_dateEdit_userDateChanged(const QDate &date);

private:
    Ui::RecentCustomers *ui;
};

#endif // RECENTCUSTOMERS_H
