#include "outstandingchequespurchie.h"
#include "ui_outstandingchequespurchie.h"
#include <QPaintEngine>
#include <QPrintEngine>

OutStandingChequesPurchie::OutStandingChequesPurchie(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OutStandingChequesPurchie)
{
    ui->setupUi(this);
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(loadParams()) );
    timer->start(5);
}

OutStandingChequesPurchie::~OutStandingChequesPurchie()
{
    delete ui;
}


void OutStandingChequesPurchie::autocompleter(QString sql, QLineEdit *name_txt, QLineEdit *id_txt)
{
    sch.name_txt = name_txt;
    sch.id_txt = id_txt;
    QMap<int, QString> data = sch.data;
    QSqlQuery qry;
    qry.prepare(sql);
    if(qry.exec())
    {
        while(qry.next())
        {
            data[qry.value(0).toInt()] = qry.value(1).toString();
        }
    }
    QCompleter *completer = new QCompleter(this);
    QStandardItemModel *model = new QStandardItemModel(completer);
    QMapIterator<int, QString> it(data);
    while (it.hasNext())
    {
        it.next();
        int code = it.key();
        QString name = it.value();
        QStandardItem *item = new QStandardItem;
        item->setText(name);
        item->setData(code, Qt::UserRole);
        model->appendRow(item);
    }
    completer->setModel(model);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setCurrentRow(0);
    completer->setFilterMode(Qt::MatchContains);
    name_txt->setCompleter(completer);


    connect(completer, SIGNAL(highlighted(QModelIndex)),this,SLOT(onItemHighlighted(QModelIndex)),Qt::QueuedConnection);
    connect(name_txt,SIGNAL(editingFinished()),this,SLOT(editingFinished() ));
}

void OutStandingChequesPurchie::onItemHighlighted(const QModelIndex &index)
{
    QString code = index.data(Qt::UserRole).toString();
    QString sname = index.data(0).toString();
    sch.searchname = sname;
    sch.searchid = code;
    sch.id_txt->setText(code);
}
void OutStandingChequesPurchie::editingFinished()
{
    QString sname = sch.name_txt->text();
    QString sid = sch.id_txt->text();
    if(sname!=sch.searchname || sid != sch.searchid)
    {
        sch.name_txt->setText("");
        sch.id_txt->setText("");
    }
}

void OutStandingChequesPurchie::loadParams()
{
    timer->stop();
    ui->detailTable->setColumnCount(8);
    QStringList titles;
    titles <<"Date"<<"Voucher #"<<"Pay Method"<<"Cheque/\nPurchie #"<<"Customer"<<"Description"<<"Amount"<<"Due Date";
    ui->detailTable->setHorizontalHeaderLabels(titles);
    ui->detailTable->horizontalHeader()->setSectionResizeMode(CUSTOMER, QHeaderView::Stretch);
    ui->detailTable->setColumnWidth(DATE,70);
    ui->detailTable->setColumnWidth(VOUCHER,70);
    ui->detailTable->setColumnWidth(PMETHOD,70);
    ui->detailTable->setColumnWidth(CHEQUENUM,70);
    ui->detailTable->setColumnWidth(DESCRIPTION,150);
    ui->detailTable->setColumnWidth(AMOUNT,90);
    ui->detailTable->setColumnWidth(DUEDATE,70);
}

void OutStandingChequesPurchie::on_customername_textEdited(const QString &arg1)
{
    QString sql = sch.customers(arg1);
    autocompleter(sql,ui->customername,ui->customerid);
}

void OutStandingChequesPurchie::on_nextButton_clicked()
{
    this->setCursor(Qt::WaitCursor);

    ui->detailTable->setRowCount(0);
    if(ui->customerid->text()=="")
    {
        ui->lbl_customer->setText("All");
        customerid = "0";
    }
    else
    {
        ui->lbl_customer->setText( ui->customername->text() );
        customerid = ui->customerid->text();
    }

    QString customername = "All";
    if(customerid.toInt()>0)
    {
        customername = erp.customerName(customerid);
    }
    html = "<table width='100%' style='font-size:10px;' border=1 bordercolor='#333333' cellspacing='0' cellpadding='3'>"
           "<thead>"
           "<tr>"
           "<th colspan='8'>"
               "<table width='100%'>"
                   "<tr>"
                       "<th width='50%' align='left'>Customer: "+customername+"</th>"
                   "</tr>"
               "</table>"
           "</th>"
           "</tr>"
            //<<"Date"<<"Voucher #"<<"Pay Method"<<"Cheque/Purchie #"<<"Customer"<<"Description"<<"Amount"<<"Due Date";
           "<tr bgcolor='#EEEEEE'>"
           "<th width='6%'>Date</th>"
            "<th width='6%'>Voucher #</th>"
           "<th width='6%'>Pay Method</th>"
           "<th width='6%'>Cheque/Purchie #</th>"
           "<th width='40%'>Customer</th>"
           "<th width='20%'>Description</th>"
           "<th width='10%'>Amount</th>"
           "<th width='6%'>Due Date</th>"
           "</tr>"
           "</thead>"
           "<tbody>";

    float tot_amount = 0.00;

    QString query = "select * from tblgltransvoucher"
                    " inner join tblinvoicepaymentdetail on (tblgltransvoucher.voucherno = "
                    " tblinvoicepaymentdetail.voucherid) where voucherno like 'pmt%' "
                    " and tblinvoicepaymentdetail.cancel =0 and tblinvoicepaymentdetail.paymeth!='1' "
                    " and tblinvoicepaymentdetail.status in ('0','1')";
    if(customerid.toInt()>0)
    {
        query += " and customer_id = '"+customerid+"' ";
    }
    query += " order by dueDate asc ";
    QSqlQuery info(query);
    //int i;
    int row;
    while(info.next())
    {
        QString voucherid = info.value("voucherid").toString();
        QSqlQuery row2("select sum(amount) as amount from tblencashpaymentdetail where pmtno = '"+voucherid+"'");
        row2.next();

        float bal_amount = info.value("amount").toFloat() - row2.value("amount").toFloat();
        QString datereceived = info.value("datereceived").toDate().toString("dd/MM/yyyy");
        QString method = erp.paymethod( info.value("paymeth").toString() );
        QString checkNum = info.value("checkNum").toString();
        QString cust_name = erp.customerName( info.value("customer_id").toString() );
        QString bank = info.value("bank").toString();
        QString dueDate = info.value("dueDate").toDate().toString("dd/MM/yyyy");

        ui->detailTable->insertRow(ui->detailTable->rowCount());
        row = ui->detailTable->rowCount()-1;

        QTableWidgetItem *dt = new QTableWidgetItem( datereceived );
        dt->setTextAlignment(Qt::AlignCenter);

        QTableWidgetItem *vid = new QTableWidgetItem( voucherid );
        vid->setTextAlignment(Qt::AlignCenter);

        QTableWidgetItem *pmode = new QTableWidgetItem( method );
        pmode->setTextAlignment(Qt::AlignLeft|Qt::AlignVCenter);

        QTableWidgetItem *chkno = new QTableWidgetItem( checkNum );
        chkno->setTextAlignment(Qt::AlignLeft|Qt::AlignVCenter);

        QTableWidgetItem *cust = new QTableWidgetItem( cust_name );
        cust->setTextAlignment(Qt::AlignCenter);

        QTableWidgetItem *desc = new QTableWidgetItem( bank );
        desc->setTextAlignment(Qt::AlignCenter);

        QTableWidgetItem *amt = new QTableWidgetItem( erp.amountString(bal_amount) );
        amt->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);

        QTableWidgetItem *ddate = new QTableWidgetItem( dueDate );
        ddate->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);

        ui->detailTable->setItem(row, DATE,  dt );
        ui->detailTable->setItem(row, VOUCHER,     vid );
        ui->detailTable->setItem(row, PMETHOD, pmode );
        ui->detailTable->setItem(row, CHEQUENUM,  chkno );
        ui->detailTable->setItem(row, CUSTOMER,   cust );
        ui->detailTable->setItem(row, DESCRIPTION,      desc );
        ui->detailTable->setItem(row, AMOUNT,   amt );
        ui->detailTable->setItem(row, DUEDATE, ddate );

        html += "<tr style='font-size:10px;'>"
                "<td align='center'>"+datereceived+"</td>"
                "<td align='center'>"+voucherid+"</td>"
                "<td align='center'>"+method+"</td>"
                "<td align='center'>"+checkNum+"</td>"
                "<td>"+cust_name+"</td>"
                "<td>"+bank+"</td>"
                "<td align='right'>"+erp.amountString(bal_amount)+"</td>"
                "<td align='center'>"+dueDate+"</td>"
                "</tr>";

        tot_amount += bal_amount;
        //i = ui->detailTable->rowCount();
    }//end while(info_sale.next())

    ui->detailTable->insertRow(ui->detailTable->rowCount());
    row = ui->detailTable->rowCount()-1;

    QFont font("Segoe UI", 9, QFont::Bold);
    QTableWidgetItem *tottitle = new QTableWidgetItem( "Total:" );
    tottitle->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
    tottitle->setFont(font);

    QTableWidgetItem *totamt = new QTableWidgetItem( erp.amountString(tot_amount) );
    totamt->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
    totamt->setFont(font);

    QTableWidgetItem *totddate = new QTableWidgetItem( "" );

    ui->detailTable->setSpan(row, DATE,1,6);
    ui->detailTable->setItem(row, AMOUNT,    totamt );
    ui->detailTable->setItem(row, DUEDATE,  totddate );

    html += "<tr>"
            "<th align='right' colspan='6'>Total:</th>"
            "<th align='center'>"+erp.amountString(tot_amount)+"</th>"
            "<th align='right'></th>"
            "</tr>";

    html += "</tbody></table>";
    this->setCursor(Qt::ArrowCursor);
}

void OutStandingChequesPurchie::on_printButton_clicked()
{
    this->setCursor(Qt::WaitCursor);
    printer=new QPrinter();
    printer->setOutputFormat(QPrinter::PdfFormat);
    printer->setPaperSize(QPrinter::A4);
    printer->setPageMargins(QMarginsF(10,10,10,10));
    printDlg=new QPrintDialog(printer);
    pageDlg=new QPageSetupDialog(printer);

    pd = new QPrintPreviewDialog(printer);
    connect(pd,SIGNAL(paintRequested(QPrinter*)),this,SLOT(printPreview(QPrinter*)));
    pd->setWindowTitle("Print Sales Invoice");
    pd->setWindowFlags(Qt::Window);
    pd->resize(900,600);
    pd->show();
}

void OutStandingChequesPurchie::printPreview(QPrinter *p)
{
/******************************************* Header****************************************************/

    QString header_html = "<table width='100%'>"
                          "<tr>"
                          "<td rowspan='2' width='20%' valign='bottom'>Print Date: "+QDate::currentDate().toString("dd/MM/yyyy")+"</td>"
                          "<td width='60%' align='center' style='font-size:22px;text-transform:uppercase;'><b>"+erp.companyname()+"</b></td>"
                          "<td rowspan='2' width='20%' align='right' valign='bottom'></td>"
                          "</tr>"
                          "<tr>"
                          "<th style='font-size:16px;'>Summary of Invoices</th>"
                          "</tr>"
                          "<!--<tr><td colspan='3'><hr/></td></tr>-->"                                                                                                ""                                                                                               ""
                          "</table>";


/******************************************* Settings ****************************************************/

    QRect printer_rect(p->pageRect());

    //Setting up the header and calculating the header size
        QTextDocument *document_header = new QTextDocument(this);
        document_header->setPageSize(printer_rect.size());
        //int pagenum = document_header->pageCount();
        document_header->setHtml(header_html);
        QSizeF header_size = document_header->size();

    //Setting up the footer and calculating the footer size
        QTextDocument *document_footer = new QTextDocument(this);
        document_footer->setPageSize(printer_rect.size());
        //document_footer->setHtml("");
        //QSizeF footer_size = document_footer->size();

    //Calculating the main document size for one page
        QSizeF center_size(printer_rect.width(), (p->pageRect().height() - header_size.toSize().height()  ));//- footer_size.toSize().height()

    //Setting up the center page
        QTextDocument *main_doc = new QTextDocument(this);
        main_doc->setHtml(html);
        main_doc->setPageSize(center_size);


    //Setting up the rectangles for each section.
        QRect pageNo = QRect(700, 40, 100, 50);
        QRect headerRect = QRect(QPoint(0,0), document_header->size().toSize());
        QRect footerRect = QRect(QPoint(0,0), document_footer->size().toSize());
        QRect contentRect = QRect(QPoint(0,0), main_doc->size().toSize());    // Main content rectangle.
        QRect currentRect = QRect(QPoint(0,0), center_size.toSize());        // Current main content rectangle.

        QPainter painter(p);
            pagenum=1;
            while (currentRect.intersects(contentRect))
            {//Loop if the current content rectangle intersects with the main content rectangle.
                //Resetting the painter matrix co ordinate system.
                painter.resetMatrix();
                //Applying negative translation of painter co-ordinate system by current main content rectangle top y coordinate.
                painter.translate(0, -currentRect.y());
                //Applying positive translation of painter co-ordinate system by header hight.
                painter.translate(0, headerRect.height());
                //Drawing the center content for current page.
                main_doc->drawContents(&painter, currentRect);
                //Resetting the painter matrix co ordinate system.
                painter.resetMatrix();
                //Drawing the header on the top of the page
                document_header->drawContents(&painter, headerRect);
                painter.drawText(pageNo,"Page. "+erp.intString(pagenum));
                //Applying positive translation of painter co-ordinate system to draw the footer
                painter.translate(0, headerRect.height());
                painter.translate(0, center_size.height());
                document_footer->drawContents(&painter, footerRect);

                //Translating the current rectangle to the area to be printed for the next page
                currentRect.translate(0, currentRect.height());
                //Inserting a new page if there is till area left to be printed
                if (currentRect.intersects(contentRect))
                {
                    p->newPage();
                    pagenum++;
                }
            }
this->setCursor(Qt::ArrowCursor);
}
