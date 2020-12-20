#ifndef EDITCHEQUEPARCHIEENCASHED_H
#define EDITCHEQUEPARCHIEENCASHED_H

#include <QWidget>
#include "dbcon.h"
#include "erp.h"
#include <QCompleter>
#include "sch.h"

namespace Ui {
class EditChequeParchieEncashed;
}

class EditChequeParchieEncashed : public QWidget
{
    Q_OBJECT

public:
    QString loginid;
    DbCon conn;
    ERP erp;
    SCH sch;
    explicit EditChequeParchieEncashed(QWidget *parent = 0);
    ~EditChequeParchieEncashed();

private slots:
    void autocompleter(QString sql, QLineEdit *txt1, QLineEdit *txt2);
    void onItemHighlighted(const QModelIndex &index);
    void editingFinished();


    void on_depositdescription_textEdited(const QString &arg1);

    void on_paymeth_currentIndexChanged(int index);

    void on_voucherno_currentIndexChanged(int index);

    void on_method_currentIndexChanged(int index);

    void on_saveButton_clicked();

    void on_search_btn_clicked();

    void on_search_customername_textEdited(const QString &arg1);

    void on_EditButton_clicked();

    void on_DeleteButton_clicked();

private:
    Ui::EditChequeParchieEncashed *ui;
    enum columns{
        VNO,DATE,CUSTOMER,AMOUNT
    };
    void loadForm();
    void clearForm();
    void loadSearchForm();
};

#endif // EDITCHEQUEPARCHIEENCASHED_H
