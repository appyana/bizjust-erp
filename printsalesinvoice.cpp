#include "printsalesinvoice.h"
#include "ui_printsalesinvoice.h"
#include <QDebug>
#include <QPaintEngine>
#include <QPrintEngine>

PrintSalesInvoice::PrintSalesInvoice(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PrintSalesInvoice)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);
    ui->begindate->setDate(QDate::currentDate());
    ui->enddate->setDate(QDate::currentDate());
    settableHeaders();
}

PrintSalesInvoice::~PrintSalesInvoice()
{
    delete ui;
}

void PrintSalesInvoice::autocompleter(QString sql, QLineEdit *name_txt, QLineEdit *id_txt)
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

void PrintSalesInvoice::onItemHighlighted(const QModelIndex &index)
{
    QString code = index.data(Qt::UserRole).toString();
    QString sname = index.data(0).toString();
    sch.searchname = sname;
    sch.searchid = code;
    sch.id_txt->setText(code);
}
void PrintSalesInvoice::editingFinished()
{
    QString sname = sch.name_txt->text();
    QString sid = sch.id_txt->text();
    if(sname!=sch.searchname || sid != sch.searchid)
    {
        sch.name_txt->setText("");
        sch.id_txt->setText("");
    }
}

void PrintSalesInvoice::settableHeaders()
{
    QStringList titles;
    ui->tableWidgetList->setColumnCount(7);
    titles <<"ID"<<"Invoice #"<<"Ref No."<<"Date"<<"Amount"<<"Customer"<<"Notes";
    ui->tableWidgetList->setHorizontalHeaderLabels(titles);
    ui->tableWidgetList->hideColumn(INVOICEID);
    ui->tableWidgetList->setColumnWidth(INVOICEID,80);
    ui->tableWidgetList->setColumnWidth(INVOICENO,100);
    ui->tableWidgetList->setColumnWidth(REFNO,100);
    ui->tableWidgetList->setColumnWidth(INVDATE,100);
    ui->tableWidgetList->setColumnWidth(INVAMOUNT,100);
    ui->tableWidgetList->horizontalHeader()->setSectionResizeMode(CUSTOMER,QHeaderView::Stretch);
    ui->tableWidgetList->horizontalHeader()->setSectionResizeMode(NOTES,QHeaderView::Stretch);
}

void PrintSalesInvoice::on_customername_textEdited(const QString &arg1)
{
    QString str = sch.customers(arg1);
    autocompleter(str, ui->customername, ui->customerid);
}

void PrintSalesInvoice::on_btn_search_clicked()
{
    ui->tableWidgetList->setRowCount(0);
    QString sql = "SELECT * from tblinvoice where invoicenumber like 'si-%' and invoicedate BETWEEN '"+ui->begindate->date().toString("yyyy-MM-dd")+"' AND '"+ui->enddate->date().toString("yyyy-MM-dd")+"' ";
    if(ui->invoiceNo->text()!="")
    {
        sql += " AND invoicenumber = '"+ui->invoiceNo->text()+"' ";
    }
    sql += " order by invoicedate desc ";
    QSqlQuery qry;
    qry.prepare(sql);
    if(qry.exec())
    {
        int i=0;
        while(qry.next())
        {
            ui->tableWidgetList->insertRow(ui->tableWidgetList->rowCount());
            ui->tableWidgetList->setItem(i,INVOICEID, new QTableWidgetItem(qry.value("id").toString()+"aa"));
            ui->tableWidgetList->setItem(i,INVOICENO, new QTableWidgetItem(qry.value("invoicenumber").toString()));
            ui->tableWidgetList->setItem(i,REFNO, new QTableWidgetItem(qry.value("customerbillcode").toString()));
            ui->tableWidgetList->setItem(i,INVDATE, new QTableWidgetItem( erp.DDMMYYDateFromSQL( qry.value("invoicedate").toString()) ) );
            ui->tableWidgetList->setItem(i,INVAMOUNT, new QTableWidgetItem( erp.DecimalString( qry.value("net_total").toFloat() )) );
            ui->tableWidgetList->setItem(i,CUSTOMER, new QTableWidgetItem( erp.customerName( qry.value("customerid").toString()) ) );
            ui->tableWidgetList->setItem(i,NOTES, new QTableWidgetItem( erp.customerName( qry.value("notes").toString()) ) );
            i++;
        }
    }

}

void PrintSalesInvoice::on_btn_printInvoice_clicked()
{
    int searchrow = ui->tableWidgetList->currentRow();
    if(searchrow>=0)
    {
        loadInvoicePrint();
    }
    else
    {
        QMessageBox::warning(this,"Warning","Please Select invoice to print");
    }
}
void PrintSalesInvoice::loadInvoicePrint()
{
    //ted = new QTextDocument;
    printer=new QPrinter();
    //printer->setOutputFormat(QPrinter::NativeFormat);
    printer->setOutputFormat(QPrinter::PdfFormat);
    printer->setPaperSize(QPrinter::A4);
    //printer->newPage();
    //printer->setFullPage(true);
    printer->setPageMargins(QMarginsF(10,10,10,10));
    //printer->setResolution(300);
    //printer->setResolution(QPrinter::HighResolution);
   // QSizeF paperSize = printer->paperSize(QPrinter::Point);

//    QSizeF paperSize;
//    paperSize.setWidth(printer->width());
//    paperSize.setHeight(printer->height());

//    ted->setPageSize(paperSize);
//    ted->setDefaultStyleSheet(""
//                              "table, th, td { font-size:12pt; white-space: nowrap; border: 1px solid black; border-collapse: collapse; padding: 2px 4px; text-align: center;}"
//                              "");

    //ted->setPageSize(printer->pageSizeMM());

    printDlg=new QPrintDialog(printer);
    pageDlg=new QPageSetupDialog(printer);



//    preview = new QPrintPreviewWidget (printer);
//    this->ui->verticalLayout_3->addWidget(preview);
//    preview->setZoomMode(QPrintPreviewWidget::FitInView);

//    connect(preview, SIGNAL(paintRequested(QPrinter*)),this,SLOT(printPreview(QPrinter*)));//Associate print preview content

//    preview->setZoomFactor(1);
//    preview->show();
    //UpdateDoc();
    //preview->print();

      //QPrintPreviewWidget
    pd = new QPrintPreviewDialog(printer);
    connect(pd,SIGNAL(paintRequested(QPrinter*)),this,SLOT(printPreview(QPrinter*)));
    pd->setWindowTitle("Print Sales Invoice");
    pd->setWindowFlags(Qt::Window);
    pd->resize(900,600);
    pd->show();
    //delete pd;

    //ui->stackedWidget->setCurrentIndex(1);
}


void PrintSalesInvoice::printPreview(QPrinter *p)
{
    QString invoiceNumber = ui->tableWidgetList->item(ui->tableWidgetList->currentRow(),INVOICENO )->text();

    QString sql = " select * from tblinvoice where invoicenumber = '"+invoiceNumber+"' ";
    QSqlQuery qry;
    qry.prepare(sql);
    qry.exec();
    qry.next();

    QString customerid = qry.value("customerid").toString();
    QString soldToString = "<b>Addres:</b> "+erp.customerField(customerid,"address1")+"<br/><b>Phone:</b> "+erp.customerField(customerid,"phone1")+"<br/>"
                           "<b>Mobile:</b> "+erp.customerField(customerid,"phone3")+"<br/><b>Fax:</b> "+erp.customerField(customerid,"phone5");

/**************************************** Detail *******************************************************/
    QString html;
    html += "<table width='100%'>"
            "<tr>"
            "<td width='45%'>"
            "<table width='100%' border=1 cellspacing=0>"
            "<tr>"
            "<td>"
            "<table width='100%' style='font-size:12px;'>"
            "<tr>"
            "<td><b>Name:</b> "+erp.customerField(customerid,"name")+"<br/><b>City:</b> "+erp.customerField(customerid,"city")+"</td>"
                   "<td rowspan='2' width='10'><br/><br/><br/><br/><br/><br/></td>"
            "</tr>"
            "<tr>"
            "<td>"+soldToString+"</td>"
            "</tr>"
            "</table>"
            "</td>"
            "</tr>"
            "</table>"
            "</td>"
            "<td width='10%'></td>"
            "<td width='45%'>"
            "<table width='100%' border=1 cellspacing=0>"
            "<tr>"
            "<td>"
            "<table width='100%' style='font-size:12px;'>"
            "<tr>"
            "<td><b>Invoice Date:</b> "+qry.value("invoicedate").toDate().toString("dd/MM/yyyy")+"<br/><b>Carrier:</b> "+erp.carrierName(qry.value("Carrier_id").toString())+"</td>"
                   "<td width='10' rowspan='2'><br/><br/><br/><br/><br/><br/></td>"
            "</tr>"
            "<tr>"
            "<td><b>Billity No:</b> "+qry.value("shipped_via").toString()+"<br/><b>Ref No:</b> "+qry.value("vendorRefno").toString()+"<br/>"
            "<b>Shipto:</b> "+qry.value("shipto").toString()+""
            "</td>"
            "</tr>"
            "</table>"
            "</td>"
            "</tr>"
            "</table>"
            "</td>"
            "</tr>"
            "</table>";

    html += "<br><table width='100%' style='font-size:12px;' border=1 bordercolor='#333333' cellspacing='0' cellpadding='3'>"
            "<thead><tr bgcolor='#EEEEEE'>"
            "<th width='5%' style='font-size:12px;'>Sr.</th>"
            "<th width='12%' style='font-size:12px;'>CODE</th>"
            "<th width='39%' style='font-size:12px;'>ITEM DESCRIPTION</th>"
            "<th width='8%' style='font-size:12px;'>QTY</th>"
            "<th width='12%' style='font-size:12px;'>UNIT</th>"
            "<th width='12%' style='font-size:12px;'>PRICE</th>"
            "<th width='12%' style='font-size:12px;'>AMOUNT</th>"
            "</tr style='font-size:12px;'></thead><tbody>";

    QString sqldet = "SELECT * FROM tblinvoicedetail where invoiceid = '"+invoiceNumber+"' order by linenumber";
    QSqlQuery det;
    det.prepare(sqldet);
    int ln=1;
    float total=0.00;
    if(det.exec())
    {
        while(det.next())
        {
            html += "<tr>"
                    "<td align='center'>"+ erp.intString(ln) +"</td>"
                    "<td align='center'>"+erp.getItemField(det.value("itemid").toString(),"itemcode")+"</td>"
                    "<td>"+erp.itemname(det.value("itemid").toString())+"</td>"
                    "<td align='center'>"+erp.intString(det.value("qty").toInt())+"</td>"
                    "<td align='center'>"+erp.unitName(det.value("packingUnit").toString())+"</td>"
                    "<td align='right'>"+erp.DecimalString(det.value("priceach").toFloat())+"</td>"
                    "<td align='right'>"+erp.DecimalString(det.value("amount").toFloat())+"</td>"
                    "</tr>";
            total += det.value("amount").toFloat();
            ln++;
        }
    }
    for(int i=ln; i<=25; i++)
    {
        html += "<tr>"
                "<td align='center'>"+ erp.intString(i) +"</td>"
                "<td></td>"
                "<td></td>"
                "<td></td>"
                "<td></td>"
                "<td></td>"
                "<td></td>"
                "</tr>";
    }
    float discount = qry.value("totalDiscount").toFloat();
    float freight = qry.value("freight").toFloat();
    float nettotal = total-discount+freight;
    html += "<tr>"
            "<th colspan='5' rowspan='4'></th>"
            "<th align='right'>Total:</th>"
            "<th align='right'>"+erp.DecimalString(total)+"</th>"
            "</tr>"
            "<tr>"
            "<th align='right'>Discount:</th>"
            "<th align='right'>"+erp.DecimalString(discount)+"</th>"
            "</tr>"
            "<tr>"
            "<th align='right'>Freight:</th>"
            "<th align='right'>"+erp.DecimalString(freight)+"</th>"
            "</tr>"
            "<tr>"
            "<th align='right'>Net Total:</th>"
            "<th align='right'>"+erp.DecimalString(nettotal)+"</th>"
            "</tr>";

    html += "</tbody></table>";

/******************************************* Header****************************************************/

    QString header_html = "<table width='100%'>"
                          "<tr>"
                          "<td rowspan='2' width='20%' valign='bottom'>Print Date: "+QDate::currentDate().toString("dd/MM/yyyy")+"</td>"
                          "<td width='60%' align='center' style='font-size:22px;text-transform:uppercase;'><b>"+erp.companyname()+"</b></td>"
                          "<td rowspan='2' width='20%' align='right' valign='bottom'>Invoice #: "+invoiceNumber+"</td>"
                          "</tr>"
                          "<tr>"
                          "<th style='font-size:16px;'>Sales Invoice</th>"
                          "</tr>"
                          "<tr><td colspan='3'><hr/></td></tr>"                                                                                                ""                                                                                               ""
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
        //QRect pageNo = QRect(700, 18, 100, 50);
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
                //painter.drawText(pageNo,"Page. "+erp.intString(pagenum));
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

    //ted->setHtml(getHtmlStr());
//    ted->pageCount();
//    QPointF pageNumberPos;
//    pageNumberPos.setX(10);
//    pageNumberPos.setY(10);
//    ted->setDocumentMargin(25);


//    ted->setHtml(html);
//    ted->print(p);

}


