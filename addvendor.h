#ifndef ADDVENDOR_H
#define ADDVENDOR_H

#include <QWidget>
#include "dbcon.h"
#include "erp.h"
#include <QCompleter>
#include "sch.h"

namespace Ui {
class AddVendor;
}

class AddVendor : public QWidget
{
    Q_OBJECT

public:
    QString loginid;
    DbCon conn;
    ERP erp;
    SCH sch;
    explicit AddVendor(QWidget *parent = 0);
    ~AddVendor();

private slots:
    void autocompleter(QString sql, QLineEdit *txt1, QLineEdit *txt2);
    void onItemHighlighted(const QModelIndex &index);
    void editingFinished();

    void on_groupname_textEdited(const QString &arg1);

    void on_verbal_textEdited(const QString &arg1);

    void on_btn_save_clicked();

private:
    Ui::AddVendor *ui;
    bool isvalidate();
    void clearform();
};

#endif // ADDVENDOR_H
