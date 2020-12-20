#ifndef EDITVENDOR_H
#define EDITVENDOR_H

#include <QWidget>
#include "dbcon.h"
#include "erp.h"
#include <QCompleter>
#include "sch.h"

namespace Ui {
class EditVendor;
}

class EditVendor : public QWidget
{
    Q_OBJECT

public:
    QString loginid;
    DbCon conn;
    ERP erp;
    SCH sch;
    explicit EditVendor(QWidget *parent = 0);
    ~EditVendor();

private slots:
    void autocompleter(QString sql, QLineEdit *txt1, QLineEdit *txt2);
    void onItemHighlighted(const QModelIndex &index);
    void editingFinished();

    void on_groupname_textEdited(const QString &arg1);

    void on_verbal_textEdited(const QString &arg1);

    void on_btn_save_clicked();

    void on_vendorname_textEdited(const QString &arg1);

    void on_btn_delete_clicked();

    void on_vendorid_textChanged(const QString &arg1);

private:
    Ui::EditVendor *ui;
    bool isvalidate();
    void clearform();
    int dl=0;
};

#endif // EDITVENDOR_H
