#ifndef PURCHASERETURNLIST_H
#define PURCHASERETURNLIST_H

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
class PurchaseReturnList;
}

class PurchaseReturnList : public QWidget
{
    Q_OBJECT

public:
    QString loginid;
    DbCon conn;
    ERP erp;
    SCH sch;
    explicit PurchaseReturnList(QWidget *parent = 0);
    ~PurchaseReturnList();

private slots:
    void autocompleter(QString sql, QLineEdit *txt1, QLineEdit *txt2);
    void onItemHighlighted(const QModelIndex &index);
    void editingFinished();

    void on_vendorname_textEdited(const QString &arg1);

    void on_btn_search_clicked();

    void on_btn_printInvoice_clicked();

    //void printPreview2(QPrinter *p);
    void printPreview(QPrinter *p);
    void loadParams();

private:
    Ui::PurchaseReturnList *ui;
    //<<"Invoice #"<<"Vendor"<<"Total"<<"Date";
    enum searchcolumns{
        INVOICENO,VENDOR,TOTAL,DATE
    };
    void settableHeaders();
    void loadInvoicePrint();
    QPrinter *printer;
    QPrintDialog *printDlg;
    QPageSetupDialog *pageDlg;
    QPrintPreviewDialog *pd;
    int pagenum=0;
    QTimer *timer;
};

#endif // PURCHASERETURNLIST_H
