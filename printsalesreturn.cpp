#include "printsalesreturn.h"
#include "ui_printsalesreturn.h"
#include <QDebug>
#include <QPaintEngine>
#include <QPrintEngine>

PrintSalesReturn::PrintSalesReturn(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PrintSalesReturn)
{
    ui->setupUi(this);
    ui->begindate->setDate(QDate::currentDate());
    ui->enddate->setDate(QDate::currentDate());
    settableHeaders();
}

PrintSalesReturn::~PrintSalesReturn()
{
    delete ui;
}



void PrintSalesReturn::autocompleter(QString sql, QLineEdit *name_txt, QLineEdit *id_txt)
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

void PrintSalesReturn::onItemHighlighted(const QModelIndex &index)
{
    QString code = index.data(Qt::UserRole).toString();
    QString sname = index.data(0).toString();
    sch.searchname = sname;
    sch.searchid = code;
    sch.id_txt->setText(code);
}
void PrintSalesReturn::editingFinished()
{
    QString sname = sch.name_txt->text();
    QString sid = sch.id_txt->text();
    if(sname!=sch.searchname || sid != sch.searchid)
    {
        sch.name_txt->setText("");
        sch.id_txt->setText("");
    }
}

void PrintSalesReturn::settableHeaders()
{
    QStringList titles;
    ui->tableWidgetList->setColumnCount(5);
    titles <<"Return #"<<"Ref No."<<"Date"<<"Amount"<<"Customer";
    ui->tableWidgetList->setHorizontalHeaderLabels(titles);
    ui->tableWidgetList->setColumnWidth(RETNO,100);
    ui->tableWidgetList->setColumnWidth(REFNO,100);
    ui->tableWidgetList->setColumnWidth(RETDATE,100);
    ui->tableWidgetList->setColumnWidth(RETAMOUNT,100);
    ui->tableWidgetList->horizontalHeader()->setSectionResizeMode(CUSTOMER,QHeaderView::Stretch);
}

void PrintSalesReturn::on_customername_textEdited(const QString &arg1)
{
    QString str = sch.customers(arg1);
    autocompleter(str, ui->customername, ui->customerid);
}

void PrintSalesReturn::on_btn_search_clicked()
{
    ui->tableWidgetList->setRowCount(0);
    QString sql = "SELECT * from tblreturn where id like 'SR-%' and retdate BETWEEN '"+ui->begindate->date().toString("yyyy-MM-dd")+"' AND '"+ui->enddate->date().toString("yyyy-MM-dd")+"' ";
    if(ui->invoiceNo->text()!="")
    {
        sql += " AND (id = '"+ui->invoiceNo->text()+"' or id = 'SR-"+ui->invoiceNo->text()+"') ";
    }
    sql += " order by retdate desc ";
    QSqlQuery qry;
    qry.prepare(sql);
    if(qry.exec())
    {
        int i=0;
        while(qry.next())
        {
            ui->tableWidgetList->insertRow(ui->tableWidgetList->rowCount());
            ui->tableWidgetList->setItem(i,RETNO, new QTableWidgetItem(qry.value("id").toString()));
            ui->tableWidgetList->setItem(i,REFNO, new QTableWidgetItem(qry.value("vendorRefno").toString()));
            ui->tableWidgetList->setItem(i,RETDATE, new QTableWidgetItem( erp.DDMMYYDateFromSQL( qry.value("retdate").toString()) ) );
            ui->tableWidgetList->setItem(i,RETAMOUNT, new QTableWidgetItem( erp.DecimalString( qry.value("total_amount").toFloat() )) );
            ui->tableWidgetList->setItem(i,CUSTOMER, new QTableWidgetItem( erp.customerName( qry.value("customer_id").toString()) ) );
            i++;
        }
    }

}

void PrintSalesReturn::on_btn_printInvoice_clicked()
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
void PrintSalesReturn::loadInvoicePrint()
{
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


void PrintSalesReturn::printPreview(QPrinter *p)
{
    QString invoiceNumber = ui->tableWidgetList->item(ui->tableWidgetList->currentRow(),RETNO )->text();

    QString sql = " select * from tblreturn where id = '"+invoiceNumber+"' ";
    QSqlQuery qry;
    qry.prepare(sql);
    qry.exec();
    qry.next();

    QString customerid = qry.value("customer_id").toString();
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
            "<td><b>Return Date:</b> "+qry.value("retdate").toDate().toString("dd/MM/yyyy")+"<br/><b>Carrier:</b> "+erp.carrierName(qry.value("Carrier_id").toString())+"</td>"
                   "<td width='10' rowspan='2'><br/><br/><br/><br/><br/><br/></td>"
            "</tr>"
            "<tr>"
            "<td><b>Billity No:</b> "+qry.value("shipped_via").toString()+"<br/><b>Ref No:</b> "+qry.value("vendorRefno").toString()+" "
            ""
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

    QString sqldet = "SELECT * FROM tblreturndetail where orderid = '"+invoiceNumber+"' order by linenumber";
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
                    "<td align='center'>"+erp.intString(det.value("itemqtyorder").toInt())+"</td>"
                    "<td align='center'>"+erp.unitName(det.value("packingUnit").toString())+"</td>"
                    "<td align='right'>"+erp.DecimalString(det.value("itempriceach").toFloat())+"</td>"
                    "<td align='right'>"+erp.DecimalString(det.value("total").toFloat())+"</td>"
                    "</tr>";
            total += det.value("total").toFloat();
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
                          "<td rowspan='2' width='20%' align='right' valign='bottom'>Return #: "+invoiceNumber+"</td>"
                          "</tr>"
                          "<tr>"
                          "<th style='font-size:16px;'>Sales Return</th>"
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
