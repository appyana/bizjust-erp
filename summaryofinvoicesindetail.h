#ifndef SUMMARYOFINVOICESINDETAIL_H
#define SUMMARYOFINVOICESINDETAIL_H

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
class SummaryOfInvoicesInDetail;
}

class SummaryOfInvoicesInDetail : public QWidget
{
    Q_OBJECT

public:
    QString loginid;
    DbCon conn;
    ERP erp;
    SCH sch;
    explicit SummaryOfInvoicesInDetail(QWidget *parent = 0);
    ~SummaryOfInvoicesInDetail();

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
    Ui::SummaryOfInvoicesInDetail *ui;
    //titles <<"Invoice #"<<"Date"<<"Customer Name"<<"Carrier"<<"Bility No"<<"Qty"<<"Amount"<<"Discount"<<"Freight"<<"Net Amount";
    enum collumns{
        INVOICE,DATE,CUSTOMER,CARRIER,BILITY,QTY,AMOUNT,DISCOUNT,FREIGHT,NETAMOUNT
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

#endif // SUMMARYOFINVOICESINDETAIL_H
