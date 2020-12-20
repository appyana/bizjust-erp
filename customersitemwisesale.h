#ifndef CUSTOMERSITEMWISESALE_H
#define CUSTOMERSITEMWISESALE_H

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
class CustomersItemWiseSale;
}

class CustomersItemWiseSale : public QWidget
{
    Q_OBJECT

public:
    QString loginid;
    DbCon conn;
    ERP erp;
    SCH sch;
    explicit CustomersItemWiseSale(QWidget *parent = 0);
    ~CustomersItemWiseSale();

private slots:
    void autocompleter(QString sql, QLineEdit *txt1, QLineEdit *txt2);
    void onItemHighlighted(const QModelIndex &index);
    void editingFinished();
    void loadParams();
    void printPreview(QPrinter *p);

    void on_customername_textEdited(const QString &arg1);

    void on_btn_next_clicked();

private:
    Ui::CustomersItemWiseSale *ui;
    QString html;
    QTimer *timer;
    void loadReport();
    QPrinter *printer;
    QPrintDialog *printDlg;
    QPageSetupDialog *pageDlg;
    QPrintPreviewDialog *pd;
    int pagenum=0;
};

#endif // CUSTOMERSITEMWISESALE_H
