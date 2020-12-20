#ifndef ITEMSLIST_H
#define ITEMSLIST_H

#include <QWidget>
#include "dbcon.h"
#include "erp.h"
#include <QCompleter>
#include "sch.h"
#include <QtCore>
#include <QtGui>

namespace Ui {
class ItemsList;
}

class ItemsList : public QWidget
{
    Q_OBJECT

public:
    QString loginid;
    DbCon conn;
    ERP erp;
    SCH sch;
    explicit ItemsList(QWidget *parent = 0);
    ~ItemsList();

private:
    Ui::ItemsList *ui;
    enum columns{
        ITEMCODE,DESCRIPTION,ITEMTYPE,CATEGORY,SUBCATEGORY,BRAND,SALESPRICE
    };
    void loadform();
};

#endif // ITEMSLIST_H
