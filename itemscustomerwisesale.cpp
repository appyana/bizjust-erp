#include "itemscustomerwisesale.h"
#include "ui_itemscustomerwisesale.h"

ItemsCustomerWiseSale::ItemsCustomerWiseSale(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ItemsCustomerWiseSale)
{
    ui->setupUi(this);
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(loadParams()) );
    timer->start(5);
}

ItemsCustomerWiseSale::~ItemsCustomerWiseSale()
{
    delete ui;
}

void ItemsCustomerWiseSale::autocompleter(QString sql, QLineEdit *name_txt, QLineEdit *id_txt)
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

void ItemsCustomerWiseSale::onItemHighlighted(const QModelIndex &index)
{
    QString code = index.data(Qt::UserRole).toString();
    QString sname = index.data(0).toString();
    sch.searchname = sname;
    sch.searchid = code;
    sch.id_txt->setText(code);
}
void ItemsCustomerWiseSale::editingFinished()
{
    QString sname = sch.name_txt->text();
    QString sid = sch.id_txt->text();
    if(sname!=sch.searchname || sid != sch.searchid)
    {
        sch.name_txt->setText("");
        sch.id_txt->setText("");
    }
}

void ItemsCustomerWiseSale::loadParams()
{
    timer->stop();
    ui->begindate->setDate( erp.reportBeginDate() );
    ui->enddate->setDate( QDate::currentDate() );
}

void ItemsCustomerWiseSale::on_itemname_textEdited(const QString &arg1)
{
    autocompleter(sch.items(arg1),ui->itemname,ui->itemid);
}

void ItemsCustomerWiseSale::on_btn_next_clicked()
{
    QString itemid = ui->itemid->text();
    if(itemid=="")
    {
        QMessageBox::warning(this,"","Please Select Item Name");
        return;
    }
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

void ItemsCustomerWiseSale::printPreview(QPrinter *p)
{
    QString begindate = ui->begindate->date().toString("yyyy-MM-dd");
    QString enddate = ui->enddate->date().toString("yyyy-MM-dd");
    QString itemid = ui->itemid->text();

/******************************************report detail***********************************************/
    html = "";

    html += "<table width='100%' border='1' cellspacing='0' cellpadding='3'>"
            "<tr bgcolor='#f2f2f2'>"
            "<th colspan='7'>From: "+ erp.DDMMYYDateFromSQL(begindate) +" To: "+ erp.DDMMYYDateFromSQL(enddate) +"</th>"
            "</tr>";
    html += "<tr bgcolor='#f2f2f2'>"
            "<th colspan='7'>Item Description - Code</th>"
            "</tr>"
            "<tr>"
            "<th colspan='7'>"+erp.itemname(itemid)+" - "+erp.getItemField(itemid,"itemcode")+"</th>"
            "</tr>";

    html +="<tr bgcolor='#f2f2f2'>"
               "<th width='40%'>Customer</th>"
               "<th width='10%'>Qty</th>"
               "<th width='10%'>Amount</th>"
               "<th width='10%'>Qty Return</th>"
               "<th width='10%'>Returned</th>"
               "<th width='10%'>Net Sales</th>"
               "<th width='10%'>Avg.Price Per Unit</th>"
           "</tr>";

    int totalqty=0;
    float totalprc=0;
    float totalAvg=0;
    int totalretqty=0;
    float totalrettot=0;
    float Grandtot=0;
    QString query2= "SELECT (select `name` from tblcustomers where customerid= i.customerid) cname, i.customerid as "
                    " customer_id, avg(d.priceach) avgprice FROM tblinvoicedetail d, tblinvoice i WHERE "
                    " i.invoicenumber=d.invoiceid and i.invoicedate >= '"+begindate+"' and "
                    " i.invoicedate <= '"+enddate+"' and d.itemid='"+itemid+"' group by i.customerid order by cname asc";


    QSqlQuery info2(query2);
    while(info2.next())
    {
        QString customer_id = info2.value("customer_id").toString();
        QString customername = erp.customerName(customer_id);
        QString QUERY = "SELECT r.customer_id, r.retdate, rd.itemid, rd.packingUnit, rd.itempriceach AS eachprice, "
                        " rd.itemqtyorder AS qtyprice, SUM(rd.itempriceach) AS rditempriceach, "
                        " SUM(rd.itemqtyorder) AS rdqty, SUM(rd.itemqtyorder*rd.itempriceach) AS total "
                        " FROM tblreturn r, tblreturndetail rd WHERE r.id = rd.orderid AND rd.itemid='"+itemid+"' "
                        " AND r.customer_id='"+customer_id+"' AND r.retdate >= '"+begindate+"' and  r.retdate <= '"+enddate+"' "
                        " GROUP BY r.customer_id ";
        qDebug()<<QUERY;
        QSqlQuery row_query(QUERY);
        row_query.next();
        int qty = erp.customerItemSale_inSmallPacket(itemid,customer_id,begindate,enddate);
        float tot = qty * info2.value("avgprice").toFloat();
        float avg=0;
        if(qty!=0)
        {
            avg = tot / qty;
        }
        int retqty = row_query.value("rdqty").toInt();
        float rettot = row_query.value("total").toFloat();//retqty * row_query.value("eachprice").toFloat();
        float net_sale_amount = tot - row_query.value("total").toFloat();
        html+="<tr>"
              "<td>"+customername+"</td>"
              "<td align='center'>"+erp.amountString(qty)+"</td>"
              "<td align='right'>"+erp.amountString(tot)+"</td>"
              "<td align='center'>"+erp.amountString(retqty)+"</td>"
              "<td align='right'>"+erp.amountString(rettot)+"</td>"
              "<td align='right'>"+erp.amountString(net_sale_amount)+"</td>"
              "<td align='right'>"+erp.amountString(avg)+"</td>"
              "</tr>";
        totalqty    = totalqty    + qty;
        totalprc    = totalprc    + tot;
        totalAvg    = totalAvg    + avg;
        totalretqty = totalretqty + retqty;
        totalrettot = totalrettot + rettot;
        Grandtot    = Grandtot    + net_sale_amount;
    }

    html+="<tr>"
          "<th align='right'>Totals:</th>"
          "<th align='center'>"+erp.amountString(totalqty)+"</th>"
          "<th align='right'>"+erp.amountString(totalprc)+"</th>"
          "<th align='center'>"+erp.amountString(totalretqty)+"</th>"
          "<th align='right'>"+erp.amountString(totalrettot)+"</th>"
          "<th align='right'>"+erp.amountString(Grandtot)+"</th>"
          "<th></th>"
          "</tr>";


    html +="</table>";

/******************************************* Header****************************************************/

    QString header_html = "<table width='100%'>"
                          "<tr>"
                          "<td rowspan='2' width='20%' valign='bottom'>Print Date: "+QDate::currentDate().toString("dd/MM/yyyy")+"</td>"
                          "<td width='60%' align='center' style='font-size:22px;text-transform:uppercase;'><b>"+erp.companyname()+"</b></td>"
                          "<td rowspan='2' width='20%' align='right' valign='bottom'></td>"
                          "</tr>"
                          "<tr>"
                          "<th style='font-size:16px;'>Item's Customer Wise Sale</th>"
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
