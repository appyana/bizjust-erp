#ifndef CHARTOFACCOUNTSLIST_H
#define CHARTOFACCOUNTSLIST_H

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
class ChartofAccountsList;
}

class ChartofAccountsList : public QWidget
{
    Q_OBJECT

public:
    QString loginid;
    DbCon conn;
    ERP erp;
    SCH sch;
    explicit ChartofAccountsList(QWidget *parent = 0);
    ~ChartofAccountsList();

private slots:
    void on_btn_print_clicked();
    void printPreview(QPrinter *p);

private:
    Ui::ChartofAccountsList *ui;
    //titles <<"ACCOUNT"<<"DESCRIPTION"<<"ACCOUNT GROUP"<<"ACCOUNT TYPE"<<"SUMMARIZE TO";
    enum collumns{
        ACCOUNT,DESCRIPTION,ACCOUNTGROUP,ACCOUNTTYPE,SUMMARIZETO
    };
    QString html;
    void loadReport();
    QPrinter *printer;
    QPrintDialog *printDlg;
    QPageSetupDialog *pageDlg;
    QPrintPreviewDialog *pd;
    int pagenum=0;
};

#endif // CHARTOFACCOUNTSLIST_H
