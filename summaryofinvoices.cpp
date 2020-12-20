#include "summaryofinvoices.h"
#include "ui_summaryofinvoices.h"
#include <QPaintEngine>
#include <QPrintEngine>

SummaryOfInvoices::SummaryOfInvoices(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SummaryOfInvoices)
{
    ui->setupUi(this);
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(loadParams()) );
    timer->start(5);
}

SummaryOfInvoices::~SummaryOfInvoices()
{
    delete ui;
}


void SummaryOfInvoices::autocompleter(QString sql, QLineEdit *name_txt, QLineEdit *id_txt)
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

void SummaryOfInvoices::onItemHighlighted(const QModelIndex &index)
{
    QString code = index.data(Qt::UserRole).toString();
    QString sname = index.data(0).toString();
    sch.searchname = sname;
    sch.searchid = code;
    sch.id_txt->setText(code);
}
void SummaryOfInvoices::editingFinished()
{
    QString sname = sch.name_txt->text();
    QString sid = sch.id_txt->text();
    if(sname!=sch.searchname || sid != sch.searchid)
    {
        sch.name_txt->setText("");
        sch.id_txt->setText("");
    }
}

void SummaryOfInvoices::loadParams()
{
    timer->stop();
    ui->begindate->setDate( erp.reportBeginDate() );
    ui->enddate->setDate( QDate::currentDate() );
    ui->detailTable->setColumnCount(10);
    QStringList titles;
    titles <<"Invoice #"<<"Date"<<"Customer Name"<<"Carrier"<<"Bility No"<<"Qty"<<"Amount"<<"Discount"<<"Freight"<<"Net Amount";
    ui->detailTable->setHorizontalHeaderLabels(titles);
    ui->detailTable->horizontalHeader()->setSectionResizeMode(CUSTOMER, QHeaderView::Stretch);
    ui->detailTable->setColumnWidth(INVOICE,70);
    ui->detailTable->setColumnWidth(DATE,70);
    ui->detailTable->setColumnWidth(CARRIER,110);
    ui->detailTable->setColumnWidth(BILITY,90);
    ui->detailTable->setColumnWidth(QTY,50);
    ui->detailTable->setColumnWidth(AMOUNT,90);
    ui->detailTable->setColumnWidth(DISCOUNT,90);
    ui->detailTable->setColumnWidth(FREIGHT,90);
    ui->detailTable->setColumnWidth(NETAMOUNT,90);
}

void SummaryOfInvoices::on_customername_textEdited(const QString &arg1)
{
    QString sql = sch.customers(arg1);
    autocompleter(sql,ui->customername,ui->customerid);
}

void SummaryOfInvoices::on_nextButton_clicked()
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
    ui->lbl_begindate->setText( ui->begindate->date().toString("dd/MM/yyyy") );
    ui->lbl_enddate->setText( ui->enddate->date().toString("dd/MM/yyyy") );

    QString begindate = ui->begindate->date().toString("yyyy-MM-dd");
    QString enddate = ui->enddate->date().toString("yyyy-MM-dd");

    QString customername = "All";
    if(customerid.toInt()>0)
    {
        customername = erp.customerName(customerid);
    }
    html = "<table width='100%' style='font-size:10px;' border=1 bordercolor='#333333' cellspacing='0' cellpadding='3'>"
           "<thead>"
           "<tr>"
           "<th colspan='10'>"
               "<table width='100%'>"
                   "<tr>"
                       "<th width='50%' align='left'>Customer: "+customername+"</th>"
                       "<th width='25%' align='left'>From Date: "+erp.DDMMYYDateFromSQL(begindate)+"</th>"
                       "<th width='25%' align='left'>To Date: "+erp.DDMMYYDateFromSQL(enddate)+"</th>"
                   "</tr>"
               "</table>"
           "</th>"
           "</tr>"
            //<<"Invoice #"<<"Date"<<"Customer Name"<<"Carrier"<<"Bility No"<<"Qty"<<"Amount"<<"Discount"<<"Freight"<<"Net Amount";
           "<tr bgcolor='#EEEEEE'>"
           "<th width='4%'>Invoice #</th>"
            "<th width='6%'>Date</th>"
           "<th width='25%'>Customer Name</th>"
           "<th width='17%'>Carrier</th>"
           "<th width='6%'>Bility No</th>"
           "<th width='6%'>Qty</th>"
           "<th width='6%'>Amount</th>"
           "<th width='10%'>Discount</th>"
           "<th width='10%'>Freight</th>"
           "<th width='10%'>Net Amount</th>"
           "</tr>"
           "</thead>"
           "<tbody>";

    int tot_qty = 0;
    float tot_amount = 0.00;
    float tot_discount = 0.00;
    float tot_freight = 0.00;
    float tot_net_amount = 0.00;

    QString query = "select  *, SUBSTRING_INDEX( invoicenumber, '-', 1 ) as v_code, customerid,"
                    " invoicedate as invdate from tblinvoice "
                    " where invoicenumber LIKE '%SI-%'";
    if(customerid.toInt()>0)
    {
        query += " and customerid = '"+customerid+"' ";
    }
    if(ui->begindate->text()!="" && ui->enddate->text()!="")
    {
        query += " and invoicedate >= '"+begindate+"' and invoicedate <='"+enddate+"' ";
    }
    query += " order by v_code asc ";
    QSqlQuery info(query);
    int i;
    int row;
    while(info.next())
    {
        QString invoicenumber = info.value("invoicenumber").toString();
        QSqlQuery rowqty(" select sum(qty) as quantity, sum(amount) as gross_amt from tblinvoicedetail  where invoiceid = '"+ invoicenumber + "' ");
        rowqty.next();

        int quantity = rowqty.value("quantity").toInt();
        float gross_amt = rowqty.value("gross_amt").toFloat();
        //float bal = erp.customerBalance(customerid, info.value("invdate").toString() );
        //qDebug() <<"ok loop";

            ui->detailTable->insertRow(ui->detailTable->rowCount());
            row = ui->detailTable->rowCount()-1;

            QTableWidgetItem *invno = new QTableWidgetItem( invoicenumber );
            invno->setTextAlignment(Qt::AlignCenter);
            QTableWidgetItem *invdate = new QTableWidgetItem( info.value("invdate").toDate().toString("dd/MM/yyyy") );
            invdate->setTextAlignment(Qt::AlignCenter);

            QTableWidgetItem *cust = new QTableWidgetItem( erp.customerName(info.value("customerid").toString() ) );
            cust->setTextAlignment(Qt::AlignLeft|Qt::AlignVCenter);

            QTableWidgetItem *carrier = new QTableWidgetItem( erp.carrierName(info.value("Carrier_id").toString()) );
            carrier->setTextAlignment(Qt::AlignLeft|Qt::AlignVCenter);

            QTableWidgetItem *bility = new QTableWidgetItem( info.value("shipped_via").toString() );
            bility->setTextAlignment(Qt::AlignCenter);

            QTableWidgetItem *qty = new QTableWidgetItem( erp.amountString(quantity) );
            qty->setTextAlignment(Qt::AlignCenter);
            QTableWidgetItem *amt = new QTableWidgetItem( erp.amountString(gross_amt) );
            amt->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);

            QTableWidgetItem *disc = new QTableWidgetItem( erp.amountString(info.value("totalDiscount").toFloat()) );
            disc->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
            QTableWidgetItem *frt = new QTableWidgetItem( erp.amountString( info.value("freight").toFloat() ) );
            frt->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
            QTableWidgetItem *netamt = new QTableWidgetItem( erp.amountString( info.value("net_total").toFloat() ) );
            netamt->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);

            ui->detailTable->setItem(row, INVOICE,  invno );
            ui->detailTable->setItem(row, DATE,     invdate );
            ui->detailTable->setItem(row, CUSTOMER, cust );
            ui->detailTable->setItem(row, CARRIER,  carrier );
            ui->detailTable->setItem(row, BILITY,   bility );
            ui->detailTable->setItem(row, QTY,      qty );
            ui->detailTable->setItem(row, AMOUNT,   amt );
            ui->detailTable->setItem(row, DISCOUNT, disc );
            ui->detailTable->setItem(row, FREIGHT,  frt );
            ui->detailTable->setItem(row, NETAMOUNT,netamt );

            html += "<tr style='font-size:10px;'>"
                    "<td>"+invoicenumber+"</td>"
                    "<td>"+info.value("invdate").toDate().toString("dd/MM/yyyy")+"</td>"
                    "<td>"+erp.customerName(info.value("customerid").toString() )+"</td>"
                    "<td>"+erp.carrierName(info.value("Carrier_id").toString())+"</td>"
                    "<td align='center'>"+info.value("shipped_via").toString()+"</td>"
                    "<td align='center'>"+erp.amountString(quantity)+"</td>"
                    "<td align='right'>"+erp.amountString(gross_amt)+"</td>"
                    "<td align='right'>"+erp.amountString(info.value("totalDiscount").toFloat())+"</td>"
                    "<td align='right'>"+erp.amountString( info.value("freight").toFloat() )+"</td>"
                    "<td align='right'>"+erp.amountString( info.value("net_total").toFloat() )+"</td>"
                    "</tr>";

            tot_qty += quantity;
            tot_amount += gross_amt;
            tot_discount += info.value("totalDiscount").toFloat();
            tot_freight += info.value("freight").toFloat();
            tot_net_amount += info.value("net_total").toFloat();
            i = ui->detailTable->rowCount();
    }//end while(info_sale.next())

    ui->detailTable->insertRow(ui->detailTable->rowCount());
    row = ui->detailTable->rowCount()-1;

    QFont font("Segoe UI", 9, QFont::Bold);
    QTableWidgetItem *tottitle = new QTableWidgetItem( "Total Invoices:"+erp.intString(i) );
    tottitle->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
    tottitle->setFont(font);
    QTableWidgetItem *totqty = new QTableWidgetItem( erp.intString(tot_qty) );
    totqty->setTextAlignment(Qt::AlignCenter);
    totqty->setFont(font);

    QTableWidgetItem *totamt = new QTableWidgetItem( erp.amountString(tot_amount) );
    totamt->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
    totamt->setFont(font);

    QTableWidgetItem *totdisc = new QTableWidgetItem( erp.amountString(tot_discount) );
    totdisc->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
    totdisc->setFont(font);

    QTableWidgetItem *totfrt = new QTableWidgetItem( erp.amountString(tot_freight) );
    totfrt->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
    totfrt->setFont(font);

    QTableWidgetItem *totnet = new QTableWidgetItem( erp.amountString(tot_net_amount) );
    totnet->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
    totnet->setFont(font);

    ui->detailTable->setSpan(row, INVOICE,1,5);
    ui->detailTable->setItem(row, INVOICE,   tottitle );
    ui->detailTable->setItem(row, QTY,       totqty );
    ui->detailTable->setItem(row, AMOUNT,    totamt );
    ui->detailTable->setItem(row, DISCOUNT,  totdisc );
    ui->detailTable->setItem(row, FREIGHT,   totfrt );
    ui->detailTable->setItem(row, NETAMOUNT, totnet );

    html += "<tr>"
            "<th align='right' colspan='5'>Total Invoices:"+erp.intString(i)+"</th>"
            "<th align='right'>"+erp.intString(tot_qty)+"</th>"
            "<th align='center'>"+erp.amountString(tot_amount)+"</th>"
            "<th align='right'>"+erp.amountString(tot_discount)+"</th>"
            "<th align='right'>"+erp.amountString(tot_freight)+"</th>"
            "<th align='right'>"+erp.amountString(tot_net_amount)+"</th>"
            "</tr>";

    html += "</tbody></table>";
    this->setCursor(Qt::ArrowCursor);
}

void SummaryOfInvoices::on_printButton_clicked()
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

void SummaryOfInvoices::printPreview(QPrinter *p)
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
