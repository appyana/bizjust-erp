#ifndef ITEMSCUSTOMERWISESALE_H
#define ITEMSCUSTOMERWISESALE_H

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
class ItemsCustomerWiseSale;
}

class ItemsCustomerWiseSale : public QWidget
{
    Q_OBJECT

public:
    QString loginid;
    DbCon conn;
    ERP erp;
    SCH sch;
    explicit ItemsCustomerWiseSale(QWidget *parent = 0);
    ~ItemsCustomerWiseSale();

private slots:
    void autocompleter(QString sql, QLineEdit *txt1, QLineEdit *txt2);
    void onItemHighlighted(const QModelIndex &index);
    void editingFinished();
    void loadParams();
    void printPreview(QPrinter *p);

    void on_itemname_textEdited(const QString &arg1);

    void on_btn_next_clicked();

private:
    Ui::ItemsCustomerWiseSale *ui;
    QString html;
    QTimer *timer;
    void loadReport();
    QPrinter *printer;
    QPrintDialog *printDlg;
    QPageSetupDialog *pageDlg;
    QPrintPreviewDialog *pd;
    int pagenum=0;
};

#endif // ITEMSCUSTOMERWISESALE_H
