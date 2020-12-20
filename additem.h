#ifndef ADDITEM_H
#define ADDITEM_H

#include <QWidget>
#include "dbcon.h"
#include "erp.h"
#include <QCompleter>
#include "sch.h"
#include "comboboxes.h"

namespace Ui {
class AddItem;
}

class AddItem : public QWidget
{
    Q_OBJECT

public:
    QString loginid;
    DbCon conn;
    ERP erp;
    ComboBoxes cb;
    SCH sch;
    explicit AddItem(QWidget *parent = 0);
    ~AddItem();

private slots:
    void autocompleter(QString sql, QLineEdit *txt1, QLineEdit *txt2);
    void onItemHighlighted(const QModelIndex &index);
    void editingFinished();

    void on_vendorname_textEdited(const QString &arg1);

    void on_category_currentIndexChanged(int index);

    void on_btn_save_clicked();

private:
    Ui::AddItem *ui;
    void loadform();
    bool isvalidate();
    void clearform();
};

#endif // ADDITEM_H
