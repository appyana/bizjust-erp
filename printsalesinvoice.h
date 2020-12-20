#ifndef PRINTSALESINVOICE_H
#define PRINTSALESINVOICE_H

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
class PrintSalesInvoice;
}

class PrintSalesInvoice : public QWidget
{
    Q_OBJECT

public:
    QString loginid;
    DbCon conn;
    ERP erp;
    SCH sch;
    explicit PrintSalesInvoice(QWidget *parent = 0);
    ~PrintSalesInvoice();

private slots:
    void autocompleter(QString sql, QLineEdit *txt1, QLineEdit *txt2);
    void onItemHighlighted(const QModelIndex &index);
    void editingFinished();

    void on_customername_textEdited(const QString &arg1);

    void on_btn_search_clicked();

    void on_btn_printInvoice_clicked();

    //void printPreview2(QPrinter *p);
    void printPreview(QPrinter *p);

private:
    Ui::PrintSalesInvoice *ui;
    enum searchcolumns{
        INVOICEID,INVOICENO,REFNO,INVDATE,INVAMOUNT,CUSTOMER,NOTES
    };
    void settableHeaders();
    void loadInvoicePrint();
   // QString m_strPeintHtml;
    //QString getHtmlStr();
    QPrinter *printer;
    //QPrinter *p;
    //QTextDocument *ted;
    QPrintDialog *printDlg;
    QPageSetupDialog *pageDlg;
    QPrintPreviewDialog *pd;
    int pagenum=0;
    //QPrintPreviewWidget *preview;
    //void UpdateDoc();
    //void printPainter(bool isPDF,QString id,QString name,QString gender, QString age,QString mobile,QString con,QString spic1,QString spic2);
    //QString getPicFile();
};

#endif // PRINTSALESINVOICE_H
