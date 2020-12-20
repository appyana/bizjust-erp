#ifndef DAYBOOK_H
#define DAYBOOK_H

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
class DayBook;
}

class DayBook : public QWidget
{
    Q_OBJECT

public:
    QString loginid;
    DbCon conn;
    ERP erp;
    SCH sch;
    explicit DayBook(QWidget *parent = 0);
    ~DayBook();

private slots:
    void printPreview(QPrinter *p);
    void on_btn_print_clicked();

    void on_date_userDateChanged(const QDate &date);

private:
    Ui::DayBook *ui;
    enum columns{ VNO,ACCOUNT,DESCRIPTION,ENTRYDATE,RECEIPTS,PAYMENTS,BALANCE };
    QString html;
    QTimer *timer;
    void loadReport();
    QPrinter *printer;
    QPrintDialog *printDlg;
    QPageSetupDialog *pageDlg;
    QPrintPreviewDialog *pd;
    int pagenum=0;
};

#endif // DAYBOOK_H
