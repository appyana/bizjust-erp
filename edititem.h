#ifndef EDITITEM_H
#define EDITITEM_H

#include <QWidget>
#include "dbcon.h"
#include "erp.h"
#include <QCompleter>
#include "sch.h"
#include "comboboxes.h"

namespace Ui {
class EditItem;
}

class EditItem : public QWidget
{
    Q_OBJECT

public:
    QString loginid;
    DbCon conn;
    ERP erp;
    ComboBoxes cb;
    SCH sch;
    explicit EditItem(QWidget *parent = 0);
    ~EditItem();

private slots:
    void autocompleter(QString sql, QLineEdit *txt1, QLineEdit *txt2);
    void onItemHighlighted(const QModelIndex &index);
    void editingFinished();

    void on_vendorname_textEdited(const QString &arg1);

    void on_category_currentIndexChanged(int index);

    void on_btn_save_clicked();

    void on_itemname_textEdited(const QString &arg1);

    void on_itemid_textChanged(const QString &arg1);

    void on_btn_delete_clicked();

private:
    Ui::EditItem *ui;
    void loadform();
    bool isvalidate();
    void clearform();
};

#endif // EDITITEM_H
