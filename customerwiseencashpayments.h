#ifndef CUSTOMERWISEENCASHPAYMENTS_H
#define CUSTOMERWISEENCASHPAYMENTS_H

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
class CustomerWiseEncashPayments;
}

class CustomerWiseEncashPayments : public QWidget
{
    Q_OBJECT

public:
    QString loginid;
    DbCon conn;
    ERP erp;
    SCH sch;
    explicit CustomerWiseEncashPayments(QWidget *parent = 0);
    ~CustomerWiseEncashPayments();

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
    Ui::CustomerWiseEncashPayments *ui;
    //<<"Voucher Id"<<"Description"<<"Payment #"<<"Payment Method"<<"Amount";
    enum collumns{
        VOUCHER,DESCRIPTION,PAYMENTNUM,METHOD,AMOUNT
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

#endif // CUSTOMERWISEENCASHPAYMENTS_H
