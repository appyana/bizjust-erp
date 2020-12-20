#include "summaryofinvoicesindetail.h"
#include "ui_summaryofinvoicesindetail.h"
#include <QPaintEngine>
#include <QPrintEngine>

SummaryOfInvoicesInDetail::SummaryOfInvoicesInDetail(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SummaryOfInvoicesInDetail)
{
    ui->setupUi(this);
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(loadParams()) );
    timer->start(5);
}

SummaryOfInvoicesInDetail::~SummaryOfInvoicesInDetail()
{
    delete ui;
}

void SummaryOfInvoicesInDetail::autocompleter(QString sql, QLineEdit *name_txt, QLineEdit *id_txt)
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

void SummaryOfInvoicesInDetail::onItemHighlighted(const QModelIndex &index)
{
    QString code = index.data(Qt::UserRole).toString();
    QString sname = index.data(0).toString();
    sch.searchname = sname;
    sch.searchid = code;
    sch.id_txt->setText(code);
}
void SummaryOfInvoicesInDetail::editingFinished()
{
    QString sname = sch.name_txt->text();
    QString sid = sch.id_txt->text();
    if(sname!=sch.searchname || sid != sch.searchid)
    {
        sch.name_txt->setText("");
        sch.id_txt->setText("");
    }
}

void SummaryOfInvoicesInDetail::loadParams()
{
    timer->stop();
    ui->begindate->setDate( erp.reportBeginDate() );
    ui->enddate->setDate( QDate::currentDate() );
    ui->detailTable->setColumnCount(7);


//    QStringList titles;
//    titles <<"Invoice #"<<"Date"<<"Customer Name"<<"Carrier"<<"Bility No"<<"Qty"<<"Amount"<<"Discount"<<"Freight"<<"Net Amount";
//    ui->detailTable->setHorizontalHeaderLabels(titles);
    ui->detailTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    ui->detailTable->setColumnWidth(0,80);
    ui->detailTable->setColumnWidth(1,70);
    //ui->detailTable->setColumnWidth(2,80);
    ui->detailTable->setColumnWidth(3,90);
    ui->detailTable->setColumnWidth(4,100);
    ui->detailTable->setColumnWidth(5,100);
    ui->detailTable->setColumnWidth(6,100);

    ui->detailTable->setWordWrap(true);
    ui->detailTable->setTextElideMode(Qt::ElideMiddle);
    ui->detailTable->resizeRowsToContents();

    QFont font("Segoe UI", 9, QFont::Bold);
    int row = ui->detailTable->rowCount();
    ui->detailTable->insertRow(ui->detailTable->rowCount());
    ui->detailTable->setSpan(row,1,1,2);
    ui->detailTable->setSpan(row,4,1,3);
    ui->detailTable->setItem(row,0,new QTableWidgetItem("Invoice No"));
    ui->detailTable->setItem(row,1,new QTableWidgetItem("Customer"));
    ui->detailTable->setItem(row,3,new QTableWidgetItem("Ref No"));
    ui->detailTable->setItem(row,4,new QTableWidgetItem("Carrier"));
    ui->detailTable->item(row,0)->setBackgroundColor("#EAEAEA");
    ui->detailTable->item(row,0)->setFont(font);
    ui->detailTable->item(row,0)->setTextAlignment(Qt::AlignCenter);
    ui->detailTable->item(row,1)->setBackgroundColor("#EAEAEA");
    ui->detailTable->item(row,1)->setFont(font);
    ui->detailTable->item(row,1)->setTextAlignment(Qt::AlignCenter);
    ui->detailTable->item(row,3)->setBackgroundColor("#EAEAEA");
    ui->detailTable->item(row,3)->setFont(font);
    ui->detailTable->item(row,3)->setTextAlignment(Qt::AlignCenter);
    ui->detailTable->item(row,4)->setBackgroundColor("#EAEAEA");
    ui->detailTable->item(row,4)->setFont(font);
    ui->detailTable->item(row,4)->setTextAlignment(Qt::AlignCenter);

    row = ui->detailTable->rowCount();
    ui->detailTable->insertRow(ui->detailTable->rowCount());
    ui->detailTable->setItem(row,0,new QTableWidgetItem("Invoice Date"));
    ui->detailTable->item(row,0)->setBackgroundColor("#EAEAEA");
    ui->detailTable->item(row,0)->setFont(font);
    ui->detailTable->item(row,0)->setTextAlignment(Qt::AlignCenter);

    ui->detailTable->setItem(row,1,new QTableWidgetItem("Item Code"));
    ui->detailTable->item(row,1)->setBackgroundColor("#EAEAEA");
    ui->detailTable->item(row,1)->setFont(font);
    ui->detailTable->item(row,1)->setTextAlignment(Qt::AlignCenter);
    ui->detailTable->setItem(row,2,new QTableWidgetItem("Item Name"));
    ui->detailTable->item(row,2)->setBackgroundColor("#EAEAEA");
    ui->detailTable->item(row,2)->setFont(font);
    ui->detailTable->item(row,2)->setTextAlignment(Qt::AlignCenter);

    ui->detailTable->setItem(row,3,new QTableWidgetItem("Unit"));
    ui->detailTable->item(row,3)->setBackgroundColor("#EAEAEA");
    ui->detailTable->item(row,3)->setFont(font);
    ui->detailTable->item(row,3)->setTextAlignment(Qt::AlignCenter);

    ui->detailTable->setItem(row,4,new QTableWidgetItem("Quantity"));
    ui->detailTable->item(row,4)->setBackgroundColor("#EAEAEA");
    ui->detailTable->item(row,4)->setFont(font);
    ui->detailTable->item(row,4)->setTextAlignment(Qt::AlignCenter);

    ui->detailTable->setItem(row,5,new QTableWidgetItem("Price"));
    ui->detailTable->item(row,5)->setBackgroundColor("#EAEAEA");
    ui->detailTable->item(row,5)->setFont(font);
    ui->detailTable->item(row,5)->setTextAlignment(Qt::AlignCenter);

    ui->detailTable->setItem(row,6,new QTableWidgetItem("Amount"));
    ui->detailTable->item(row,6)->setBackgroundColor("#EAEAEA");
    ui->detailTable->item(row,6)->setFont(font);
    ui->detailTable->item(row,6)->setTextAlignment(Qt::AlignCenter);

}

void SummaryOfInvoicesInDetail::on_customername_textEdited(const QString &arg1)
{
    QString sql = sch.customers(arg1);
    autocompleter(sql,ui->customername,ui->customerid);
}

void SummaryOfInvoicesInDetail::on_nextButton_clicked()
{
    this->setCursor(Qt::WaitCursor);
    ui->detailTable->setRowCount(2);
    QFont font("Segoe UI", 9, QFont::Bold);
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
           "<th colspan='7'>"
               "<table width='100%'>"
                   "<tr>"
                       "<th width='50%' align='left'>Customer: "+customername+"</th>"
                       "<th width='25%' align='left'>From Date: "+erp.DDMMYYDateFromSQL(begindate)+"</th>"
                       "<th width='25%' align='left'>To Date: "+erp.DDMMYYDateFromSQL(enddate)+"</th>"
                   "</tr>"
               "</table>"
           "</th>"
           "</tr></thead>"
            //<<"Invoice #"<<"Date"<<"Customer Name"<<"Carrier"<<"Bility No"<<"Qty"<<"Amount"<<"Discount"<<"Freight"<<"Net Amount";
           "<tr bgcolor='#EEEEEE'>"
           "<th>Invoice No</th>"
            "<th colspan='2'>Customer</th>"
           "<th>Ref No</th>"
           "<th colspan='3'>Carrier</th>"
           "</tr>"
           "<tr bgcolor='#EEEEEE'>"
           "<th width='10%'>Invoice Date</th>"
           "<th width='10%'>Item Code</th>"
           "<th width='40%'>Item Name</th>"
           "<th width='10%'>Unit</th>"
           "<th width='10%'>Quantity</th>"
           "<th width='10%'>Price</th>"
           "<th width='10%'>Amount</th>"
           "</tr>"
           "<!--</thead>-->"
           "<tbody>";

    QString query = " select  *, SUBSTRING_INDEX( invoicenumber, '-', 1 ) as v_code, customerid,"
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
    query += " order by id DESC ";
    QSqlQuery info(query);
    //int i;
    int row;
    while(info.next())
    {
        QString invoicenumber = info.value("invoicenumber").toString();
        QString invoicedate = info.value("invoicedate").toDate().toString("dd/MM/yyyy");
        QString cutomername = erp.customerName(info.value("customerid").toString());
        QString vendorRefno = info.value("vendorRefno").toString();
        QString carrier = erp.carrierName(info.value("Carrier_id").toString());
        QString shipped_via = info.value("shipped_via").toString();
        if(shipped_via!="")
        {
            carrier = carrier+" - "+shipped_via;
        }
        ui->detailTable->insertRow(ui->detailTable->rowCount());
        row = ui->detailTable->rowCount()-1;
        ui->detailTable->setItem(row, 0,  new QTableWidgetItem(invoicenumber) );
        ui->detailTable->item(row,0)->setTextAlignment(Qt::AlignCenter);
        ui->detailTable->item(row,0)->setFont(font);
        //ui->detailTable->item(row,0)->setBackgroundColor("#f2f2f2");

        ui->detailTable->setItem(row, 1,  new QTableWidgetItem(cutomername) );
        ui->detailTable->item(row,1)->setTextAlignment(Qt::AlignCenter);
        ui->detailTable->item(row,1)->setFont(font);
        //ui->detailTable->item(row,1)->setBackgroundColor("#f2f2f2");
        ui->detailTable->setSpan(row,1,1,2);

        ui->detailTable->setItem(row, 3,  new QTableWidgetItem(vendorRefno) );
        ui->detailTable->item(row,3)->setTextAlignment(Qt::AlignCenter);
        ui->detailTable->item(row,3)->setFont(font);
        //ui->detailTable->item(row,3)->setBackgroundColor("#f2f2f2");

        ui->detailTable->setItem(row, 4,  new QTableWidgetItem(carrier) );
        ui->detailTable->item(row,4)->setTextAlignment(Qt::AlignCenter);
        ui->detailTable->item(row,4)->setFont(font);
        //ui->detailTable->item(row,4)->setBackgroundColor("#f2f2f2");
        ui->detailTable->setSpan(row,4,1,3);

        html += "<tr style='font-size:10px;'>"
                    "<td align='center'><b>"+invoicenumber+"</b></td>"
                    "<td align='center' colspan='2'><b>"+cutomername+"</b></td>"
                    "<td align='center'><b>"+vendorRefno+"</b></td>"
                    "<td align='center' colspan='3'><b>"+carrier+"</b></td>"
                "</tr>";
        float tot_amt=0.00;
        int ln=1;
        QSqlQuery infodet("select * from tblinvoicedetail  where invoiceid ='"+invoicenumber+"'");
        while(infodet.next())
        {
            QString itemid = infodet.value("itemid").toString();
            int qty = infodet.value("qty").toInt();
            QString unit = erp.unitName(infodet.value("packingUnit").toString());
            QString itemcode = erp.getItemField(itemid,"itemcode");
            QString itemdescription = infodet.value("itemdescription").toString();
            float priceach = infodet.value("priceach").toFloat();
            float amount = infodet.value("amount").toFloat();
            QString date = "";
            if(ln==1)
            {
                date = invoicedate;
            }
            ui->detailTable->insertRow(ui->detailTable->rowCount());
            row = ui->detailTable->rowCount()-1;
            ui->detailTable->setItem(row, 0,  new QTableWidgetItem( date ) );
            ui->detailTable->item(row,0)->setTextAlignment(Qt::AlignCenter);

            ui->detailTable->setItem(row, 1,  new QTableWidgetItem(itemcode) );
            ui->detailTable->item(row,1)->setTextAlignment(Qt::AlignCenter);

            ui->detailTable->setItem(row, 2,  new QTableWidgetItem(itemdescription) );

            ui->detailTable->setItem(row, 3,  new QTableWidgetItem(unit) );
            ui->detailTable->item(row,3)->setTextAlignment(Qt::AlignCenter);

            ui->detailTable->setItem(row, 4,  new QTableWidgetItem( erp.intString(qty) ) );
            ui->detailTable->item(row,4)->setTextAlignment(Qt::AlignCenter);


            ui->detailTable->setItem(row, 5,  new QTableWidgetItem( erp.amountString(priceach) ) );
            ui->detailTable->item(row,5)->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
            ui->detailTable->setItem(row, 5,  new QTableWidgetItem( "" ) );

            ui->detailTable->setItem(row, 6,  new QTableWidgetItem( erp.amountString(amount) ) );
            ui->detailTable->item(row,6)->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);

            html += "<tr style='font-size:10px;'>"
                    "<td align='center'>"+date+"</td>"
                    "<td align='center'>"+itemcode+"</td>"
                    "<td>"+itemdescription+"</td>"
                    "<td align='center'>"+unit+"</td>"
                    "<td align='center'>"+erp.intString(qty)+"</td>"
                    "<td align='right'>"+erp.amountString(priceach)+"</td>"
                    "<td align='right'>"+erp.amountString(amount)+"</td>"
                    "</tr>";
            ln++;
            tot_amt += amount;
        }
        float grandtotal = tot_amt - info.value("totalDiscount").toFloat() + info.value("freight").toFloat();
        ui->detailTable->insertRow(ui->detailTable->rowCount());
        row = ui->detailTable->rowCount()-1;


        ui->detailTable->setSpan(row, 0,1,6);
        ui->detailTable->setItem(row, 0,   new QTableWidgetItem( "Total:" ) );
        ui->detailTable->item(row,0)->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
        ui->detailTable->item(row,0)->setFont(font);
        ui->detailTable->setItem(row, 6, new QTableWidgetItem( erp.amountString(tot_amt) ) );
        ui->detailTable->item(row,6)->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
        ui->detailTable->item(row,6)->setFont(font);

        html += "<tr>"
                "<th align='right' colspan='6'>Total:</th>"
                "<th align='right'>"+erp.amountString(tot_amt)+"</th>"
                "</tr>";

        ui->detailTable->insertRow(ui->detailTable->rowCount());
        row = ui->detailTable->rowCount()-1;
        ui->detailTable->setSpan(row, 0,1,6);
        ui->detailTable->setItem(row, 0,   new QTableWidgetItem( "Discount:" ) );
        ui->detailTable->item(row,0)->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
        ui->detailTable->item(row,0)->setFont(font);
        ui->detailTable->setItem(row, 6, new QTableWidgetItem( erp.amountString( info.value("totalDiscount").toFloat() ) ) );
        ui->detailTable->item(row,6)->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
        ui->detailTable->item(row,6)->setFont(font);

        html += "<tr>"
                "<th align='right' colspan='6'>Discount:</th>"
                "<th align='right'>"+erp.amountString( info.value("totalDiscount").toFloat() )+"</th>"
                "</tr>";

        ui->detailTable->insertRow(ui->detailTable->rowCount());
        row = ui->detailTable->rowCount()-1;
        ui->detailTable->setSpan(row, 0,1,6);
        ui->detailTable->setItem(row, 0,   new QTableWidgetItem( "Freight:" ) );
        ui->detailTable->item(row,0)->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
        ui->detailTable->item(row,0)->setFont(font);
        ui->detailTable->setItem(row, 6, new QTableWidgetItem( erp.amountString( info.value("freight").toFloat() ) ) );
        ui->detailTable->item(row,6)->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
        ui->detailTable->item(row,6)->setFont(font);

        html += "<tr>"
                "<th align='right' colspan='6'>Freight:</th>"
                "<th align='right'>"+erp.amountString( info.value("freight").toFloat() )+"</th>"
                "</tr>";

        ui->detailTable->insertRow(ui->detailTable->rowCount());
        row = ui->detailTable->rowCount()-1;
        ui->detailTable->setSpan(row, 0,1,6);
        ui->detailTable->setItem(row, 0,   new QTableWidgetItem( "Grand Total:" ) );
        ui->detailTable->item(row,0)->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
        ui->detailTable->item(row,0)->setFont(font);
        ui->detailTable->setItem(row, 6, new QTableWidgetItem( erp.amountString(grandtotal) ) );
        //ui->detailTable->item(row,6)->setText( erp.amountString(tot_amt) );
        ui->detailTable->item(row,6)->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
        ui->detailTable->item(row,6)->setFont(font);

        html += "<tr>"
                "<th align='right' colspan='6'>Grand Total:</th>"
                "<th align='right'>"+erp.amountString(tot_amt)+"</th>"
                "</tr>"
                "<tr bgcolor='#F2F2F2'><td colspan='7' ></td></tr>";

        ui->detailTable->insertRow(ui->detailTable->rowCount());
        row = ui->detailTable->rowCount()-1;
        ui->detailTable->setSpan(row, 0,1,7);
        ui->detailTable->setItem(row, 0,   new QTableWidgetItem( "" ) );
        ui->detailTable->item(row,0)->setBackgroundColor("#f2f2f2");

    }//end while(info_sale.next())



    html += "</tbody></table>";
    this->setCursor(Qt::ArrowCursor);
}

void SummaryOfInvoicesInDetail::on_printButton_clicked()
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

void SummaryOfInvoicesInDetail::printPreview(QPrinter *p)
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
