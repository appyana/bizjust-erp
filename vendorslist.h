#ifndef VENDORSLIST_H
#define VENDORSLIST_H

#include <QWidget>
#include "dbcon.h"
#include "erp.h"
#include <QCompleter>
#include "sch.h"
#include <QtCore>
#include <QtGui>

namespace Ui {
class VendorsList;
}

class VendorsList : public QWidget
{
    Q_OBJECT

public:
    QString loginid;
    DbCon conn;
    ERP erp;
    SCH sch;
    explicit VendorsList(QWidget *parent = 0);
    ~VendorsList();

private:
    Ui::VendorsList *ui;
    //<<"ID"<<"Company Name"<<"Address"<<"City"<<"Phone"<<"Contact Person";
    enum columns{
        VENDORID,VENDORNAME,ADDRESS,CITY,PHONE,CONTACTPERSON
    };
    void loadform();
};

#endif // VENDORSLIST_H
