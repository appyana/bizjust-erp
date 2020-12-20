#ifndef CUSTOMERPAYMENTRECEIPTREPORT_H
#define CUSTOMERPAYMENTRECEIPTREPORT_H

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
class CustomerPaymentReceiptReport;
}

class CustomerPaymentReceiptReport : public QWidget
{
    Q_OBJECT

public:
    QString loginid;
    DbCon conn;
    ERP erp;
    SCH sch;
    explicit CustomerPaymentReceiptReport(QWidget *parent = 0);
    ~CustomerPaymentReceiptReport();

private slots:
    void autocompleter(QString sql, QLineEdit *txt1, QLineEdit *txt2);
    void onItemHighlighted(const QModelIndex &index);
    void editingFinished();

    void loadParams();

    void on_customername_textEdited(const QString &arg1);

    void on_nextButton_clicked();

    void on_printButton_clicked();
    void printPreview(QPrinter *p);

private:
    Ui::CustomerPaymentReceiptReport *ui;
    //<<"Customer"<<"Voucher #"<<"Date"<<"Invoice #"<<"Method"<<"Cheque #"<<"Cheque Date"<<"Bank"<<"Amount";
    enum collumns{
        CUSTOMER,VOUCHER,DATE,INVOICE,METHOD,CHEQUE,CHEQUEDATE,BANK,AMOUNT
    };
    QString html;
    QTimer *timer;
    void loadReport();
    QPrinter *printer;
    QPrintDialog *printDlg;
    QPageSetupDialog *pageDlg;
    QPrintPreviewDialog *pd;
    int pagenum=0;
    QString customerid;
};

#endif // CUSTOMERPAYMENTRECEIPTREPORT_H
