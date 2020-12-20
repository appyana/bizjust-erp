#ifndef CUSTOMERSBALANCE_H
#define CUSTOMERSBALANCE_H

#include <QWidget>
#include "dbcon.h"
#include "erp.h"
#include <QCompleter>
#include "sch.h"
#include <QtCore>
#include <QtGui>
#include <QtPrintSupport/qprinter.h>
#include <QPrintDialog>
#include <QPrintPreviewDialog>
#include <QTextDocument>
#include <QPrintPreviewWidget>
#include <QPrintDialog>
#include <QPageSetupDialog>
#include <QPainter>
#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>

namespace Ui {
class CustomersBalance;
}

class CustomersBalance : public QWidget
{
    Q_OBJECT

public:
    QString loginid;
    DbCon conn;
    ERP erp;
    SCH sch;
    explicit CustomersBalance(QWidget *parent = 0);
    ~CustomersBalance();

private slots:
    void autocompleter(QString sql, QLineEdit *txt1, QLineEdit *txt2);
    void onItemHighlighted(const QModelIndex &index);
    void editingFinished();

    void loadParams();

    void on_groupname_textEdited(const QString &arg1);

    void on_nextButton_clicked();

    void on_printButton_clicked();
    void printPreview(QPrinter *p);

private:
    Ui::CustomersBalance *ui;
    //titles <<"Customer Group"<<"Customer ID"<<"Customer Name"<<"Phone/Mobile"<<"Balance";
    enum collumns{
        GROUP,CUSTOMERID,CUSTOMERNAME,PHONE,BALANCE
    };
    QString html;
    QTimer *timer;
    void loadReport();
    QPrinter *printer;
    QPrintDialog *printDlg;
    QPageSetupDialog *pageDlg;
    QPrintPreviewDialog *pd;
    int pagenum=0;
};

#endif // CUSTOMERSBALANCE_H
