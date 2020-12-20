#ifndef CHEQUEPURCHIEENCASHED_H
#define CHEQUEPURCHIEENCASHED_H

#include <QWidget>
#include "dbcon.h"
#include "erp.h"
#include <QCompleter>
#include "sch.h"

namespace Ui {
class ChequePurchieEncashed;
}

class ChequePurchieEncashed : public QWidget
{
    Q_OBJECT

public:
    QString loginid;
    DbCon conn;
    ERP erp;
    SCH sch;
    explicit ChequePurchieEncashed(QWidget *parent = 0);
    ~ChequePurchieEncashed();

private slots:
    void autocompleter(QString sql, QLineEdit *txt1, QLineEdit *txt2);
    void onItemHighlighted(const QModelIndex &index);
    void editingFinished();


    void on_depositdescription_textEdited(const QString &arg1);

    void on_paymeth_currentIndexChanged(int index);

    void on_voucherno_currentIndexChanged(int index);

    void on_method_currentIndexChanged(int index);

    void on_saveButton_clicked();

private:
    Ui::ChequePurchieEncashed *ui;
    void loadForm();
    void clearForm();
};

#endif // CHEQUEPURCHIEENCASHED_H
