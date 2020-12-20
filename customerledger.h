#ifndef CUSTOMERLEDGER_H
#define CUSTOMERLEDGER_H

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
class CustomerLedger;
}

class CustomerLedger : public QWidget
{
    Q_OBJECT

public:
    QString loginid;
    DbCon conn;
    ERP erp;
    SCH sch;
    explicit CustomerLedger(QWidget *parent = 0);
    ~CustomerLedger();

private slots:
    void autocompleter(QString sql, QLineEdit *txt1, QLineEdit *txt2);
    void onItemHighlighted(const QModelIndex &index);
    void editingFinished();


    void on_customername_textEdited(const QString &arg1);

    void loadForm();

    void on_customerid_textChanged(const QString &arg1);

    void on_enddate_userDateChanged(const QDate &date);

    void on_begindate_userDateChanged(const QDate &date);

    void on_printButton_clicked();
    void printPreview(QPrinter *p);

private:
    Ui::CustomerLedger *ui;
    enum collumns{
        DATE,VOUCHER,DESCRIPTION,DEBIT,CREDIT,BALANCE
    };
    QTimer *timer;
    void loadLedger();
    QPrinter *printer;
    QPrintDialog *printDlg;
    QPageSetupDialog *pageDlg;
    QPrintPreviewDialog *pd;
    int pagenum=0;
};

#endif // CUSTOMERLEDGER_H
