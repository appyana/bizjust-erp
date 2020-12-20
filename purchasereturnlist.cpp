#include "purchasereturnlist.h"
#include "ui_purchasereturnlist.h"

PurchaseReturnList::PurchaseReturnList(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PurchaseReturnList)
{
    ui->setupUi(this);
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(loadParams()) );
    timer->start(5);
}

PurchaseReturnList::~PurchaseReturnList()
{
    delete ui;
}


void PurchaseReturnList::autocompleter(QString sql, QLineEdit *name_txt, QLineEdit *id_txt)
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

void PurchaseReturnList::onItemHighlighted(const QModelIndex &index)
{
    QString code = index.data(Qt::UserRole).toString();
    QString sname = index.data(0).toString();
    sch.searchname = sname;
    sch.searchid = code;
    sch.id_txt->setText(code);
}
void PurchaseReturnList::editingFinished()
{
    QString sname = sch.name_txt->text();
    QString sid = sch.id_txt->text();
    if(sname!=sch.searchname || sid != sch.searchid)
    {
        sch.name_txt->setText("");
        sch.id_txt->setText("");
    }
}

void PurchaseReturnList::loadParams()
{
    timer->stop();
    ui->begindate->setDate(erp.reportBeginDate());
    ui->enddate->setDate(QDate::currentDate());
    settableHeaders();
}

void PurchaseReturnList::settableHeaders()
{
    QStringList titles;
    ui->tableWidgetList->setColumnCount(4);
    titles <<"Return #"<<"Vendor"<<"Total"<<"Date";
    ui->tableWidgetList->setHorizontalHeaderLabels(titles);
    ui->tableWidgetList->setColumnWidth(INVOICENO,100);
    ui->tableWidgetList->setColumnWidth(TOTAL,100);
    ui->tableWidgetList->setColumnWidth(DATE,100);
    ui->tableWidgetList->horizontalHeader()->setSectionResizeMode(VENDOR,QHeaderView::Stretch);
}

void PurchaseReturnList::on_vendorname_textEdited(const QString &arg1)
{
    autocompleter(sch.vendors(arg1), ui->vendorname, ui->vendorid);
}

void PurchaseReturnList::on_btn_search_clicked()
{
    ui->tableWidgetList->setRowCount(0);
    QString sql = "SELECT * from tblpurchasereturn WHERE entrydate BETWEEN '"+ui->begindate->date().toString("yyyy-MM-dd")+"' AND '"+ui->enddate->date().toString("yyyy-MM-dd")+"' ";
    if(ui->invoiceNo->text()!="")
    {
        sql += " AND (prid = '"+ui->invoiceNo->text()+"' or prid = 'pr-"+ui->invoiceNo->text()+"') ";
    }
    if(ui->vendorid->text()!="")
    {
        sql += " and vendorid = '"+ui->vendorid->text()+"' ";
    }
    sql += " order by prid desc ";
    QSqlQuery qry;
    qry.prepare(sql);
    if(qry.exec())
    {
        int i=0;
        while(qry.next())
        {
            ui->tableWidgetList->insertRow(ui->tableWidgetList->rowCount());
            ui->tableWidgetList->setItem(i,INVOICENO, new QTableWidgetItem(qry.value("prid").toString()));
            ui->tableWidgetList->setItem(i,VENDOR, new QTableWidgetItem( erp.vendorname(qry.value("vendorid").toString()) ));
            ui->tableWidgetList->setItem(i,DATE, new QTableWidgetItem( erp.DDMMYYDateFromSQL( qry.value("entrydate").toString()) ) );
            ui->tableWidgetList->setItem(i,TOTAL, new QTableWidgetItem( erp.DecimalString( qry.value("totalprice").toFloat() )) );
            i++;
        }
    }

}

void PurchaseReturnList::on_btn_printInvoice_clicked()
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
void PurchaseReturnList::loadInvoicePrint()
{
    printer=new QPrinter();
    printer->setOutputFormat(QPrinter::PdfFormat);
    printer->setPaperSize(QPrinter::A4);
    printer->setPageMargins(QMarginsF(10,10,10,10));
    printDlg=new QPrintDialog(printer);
    pageDlg=new QPageSetupDialog(printer);
    pd = new QPrintPreviewDialog(printer);
    connect(pd,SIGNAL(paintRequested(QPrinter*)),this,SLOT(printPreview(QPrinter*)));
    pd->setWindowTitle("Print Purchase Return");
    pd->setWindowFlags(Qt::Window);
    pd->resize(900,600);
    pd->show();
}


void PurchaseReturnList::printPreview(QPrinter *p)
{
    QString vno = ui->tableWidgetList->item(ui->tableWidgetList->currentRow(),INVOICENO )->text();

    QString sql = " select * from tblpurchasereturn where prid = '"+vno+"' ";
    QSqlQuery qry;
    qry.prepare(sql);
    qry.exec();
    qry.next();

    QString vendorid = qry.value("vendorid").toString();
    QString soldToString = "<b>Addres:</b> "+erp.vendorField(vendorid,"address1")+"<br/><b>Phone:</b> "+erp.vendorField(vendorid,"phone1")+"<br/>"
                           "<b>Mobile:</b> "+erp.vendorField(vendorid,"phone3");

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
            "<td><b>Name:</b> "+erp.vendorField(vendorid,"vendorname")+"<br/><b>City:</b> "+erp.vendorField(vendorid,"city")+"</td>"
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
            "<table width='100%' border=0 cellspacing=0>"
            "<tr>"
            "<td>"
            "<table width='100%' style='font-size:12px;'>"
            "<tr>"
            "<td><!--<b>Invoice No:</b> "+qry.value("voucherno").toString()+"<br>--><b>Return Date:</b> "+qry.value("entrydate").toDate().toString("dd/MM/yyyy")+"</td>"
                   "<td width='10' rowspan='2'><br/><br/><br/><br/><br/><br/></td>"
            "</tr>"
            "<tr>"
            "<td><!--<b>Vendor Invoice No:</b> "+qry.value("vendor_inv_no").toString()+"-->"
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

    QString sqldet = "SELECT * FROM tblpreturndetail where prid = '"+vno+"' order by podetid";
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
                    "<td align='center'>"+erp.intString(det.value("itemqty").toInt())+"</td>"
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
    html += "<tr>"
            "<th colspan='5' rowspan='1'></th>"
            "<th align='right'>Total:</th>"
            "<th align='right'>"+erp.DecimalString(total)+"</th>"
            "</tr>";

    html += "</tbody></table>";

/******************************************* Header****************************************************/

    QString header_html = "<table width='100%'>"
                          "<tr>"
                          "<td rowspan='2' width='20%' valign='bottom'>Print Date: "+QDate::currentDate().toString("dd/MM/yyyy")+"</td>"
                          "<td width='60%' align='center' style='font-size:22px;text-transform:uppercase;'><b>"+erp.companyname()+"</b></td>"
                          "<td rowspan='2' width='20%' align='right' valign='bottom'>Invoice #: "+vno+"</td>"
                          "</tr>"
                          "<tr>"
                          "<th style='font-size:16px;'>Purchase Return</th>"
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
}
