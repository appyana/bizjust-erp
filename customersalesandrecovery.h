#ifndef CUSTOMERSALESANDRECOVERY_H
#define CUSTOMERSALESANDRECOVERY_H

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
class CustomerSalesAndRecovery;
}

class CustomerSalesAndRecovery : public QWidget
{
    Q_OBJECT

public:
    QString loginid;
    DbCon conn;
    ERP erp;
    SCH sch;
    explicit CustomerSalesAndRecovery(QWidget *parent = 0);
    ~CustomerSalesAndRecovery();

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
    Ui::CustomerSalesAndRecovery *ui;
    //titles <<"Customer"<<"Balance"<<"No. of Invoices"<<"Sales Value"<<"%"<<"Sales Return"<<"%"<<"Recovery"<<"%";
    enum collumns{
        CUSTOMER,BALANCE,INVOICES,SALES,SALESPERCENT,RETURN,RETURNPERCET,RECOVERY,RECOVERYPERCENT
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

#endif // CUSTOMERSALESANDRECOVERY_H
