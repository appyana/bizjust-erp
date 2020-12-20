#include "customersitemwisesale.h"
#include "ui_customersitemwisesale.h"

CustomersItemWiseSale::CustomersItemWiseSale(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CustomersItemWiseSale)
{
    ui->setupUi(this);
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(loadParams()) );
    timer->start(5);
}

CustomersItemWiseSale::~CustomersItemWiseSale()
{
    delete ui;
}

void CustomersItemWiseSale::autocompleter(QString sql, QLineEdit *name_txt, QLineEdit *id_txt)
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

void CustomersItemWiseSale::onItemHighlighted(const QModelIndex &index)
{
    QString code = index.data(Qt::UserRole).toString();
    QString sname = index.data(0).toString();
    sch.searchname = sname;
    sch.searchid = code;
    sch.id_txt->setText(code);
}
void CustomersItemWiseSale::editingFinished()
{
    QString sname = sch.name_txt->text();
    QString sid = sch.id_txt->text();
    if(sname!=sch.searchname || sid != sch.searchid)
    {
        sch.name_txt->setText("");
        sch.id_txt->setText("");
    }
}

void CustomersItemWiseSale::loadParams()
{
    timer->stop();
    ui->begindate->setDate( erp.reportBeginDate() );
    ui->enddate->setDate( QDate::currentDate() );
}

void CustomersItemWiseSale::on_customername_textEdited(const QString &arg1)
{
    autocompleter(sch.customers(arg1),ui->customername,ui->customerid);
}

void CustomersItemWiseSale::on_btn_next_clicked()
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

void CustomersItemWiseSale::printPreview(QPrinter *p)
{
    QString begindate = ui->begindate->date().toString("yyyy-MM-dd");
    QString enddate = ui->enddate->date().toString("yyyy-MM-dd");
    QString customerid = ui->customerid->text();
/******************************************report detail***********************************************/
    html = "";
    QString query = "SELECT DISTINCT (i.customerid) AS id, c.name as nam,sum(i.freight) as freight, "
                    " sum(i.totalDiscount) as discount, sum(i.extraDiscount) as extraDiscount  FROM tblinvoice i, "
                    " tblcustomers c WHERE i.customerid = c.customerid ";
    if(customerid!="")
    {
        query += " and i.customerid = '"+customerid+"' ";
    }
    if(begindate!="" && enddate!="")
    {
        query += " and  i.invoicedate >= '"+begindate+"' and i.invoicedate <= '"+enddate+"' ";
    }
    query += " group by i.customerid ORDER BY c.name ASC ";

    html += "<table width='100%' border='1' cellspacing='0' cellpadding='3'>"
            "<tr>"
            "<th colspan='6'>From: "+ erp.DDMMYYDateFromSQL(begindate) +" To: "+ erp.DDMMYYDateFromSQL(enddate) +"</th>"
            "</tr>";

    html +="<tr bgcolor='#f2f2f2'>"
               "<th width='10%'>Item Code</th>"
               "<th width='50%'>Description</th>"
               "<th width='10%'>Unit</th>"
               "<th width='10%'>Qty</th>"
               "<th width='10%'>Avg Price</th>"
               "<th width='10%'>Amount</th>"
           "</tr>";
    float net_total = 0;
    float tot_discount = 0;
    float totaleprice = 0;
    QSqlQuery info1(query);
    while(info1.next())
    {
        QString cid = info1.value("id").toString();
        html += "<tr bgcolor='#f2f2f2'><th colspan='6' align='left'>Customer: "+info1.value("nam").toString()+"</th></tr>";

        QString query_subcat="SELECT subcatid FROM tblinvoice inv, tblinvoicedetail d, tblitems i "
                             " WHERE inv.invoicenumber = d.invoiceid AND inv.customerid "
                             " = "+cid+" and d.itemid = i.itemid";
        if(begindate!="" && enddate!="")
        {
            query_subcat += " and inv.invoicedate  >='"+begindate+"' and inv.invoicedate  <='"+enddate+"' ";
        }
        query_subcat += " group by i.subcatid";
        QSqlQuery info_subcat(query_subcat);
        float total_value=0;
        while(info_subcat.next())
        {
            QString subcatid = info_subcat.value("subcatid").toString();
            QString subcatname = erp.subCategoryName( subcatid );
            html += "<tr bgcolor='#f2f2f2'><th colspan='6' align='left'>"+subcatname+"</th></tr>";

            QString query2 = "SELECT *, sum(qty) as tqty, sum(qty * priceach) as tprice, sum( d.amount) as eprice, "
                            " i.itemcode, d.itemid as itemid,sum(inv.freight) as fri, sum(inv.totalDiscount) as "
                            " discount, sum(inv.extraDiscount) as extraDiscount, d.packingUnit as unitid FROM tblinvoice inv, tblinvoicedetail "
                            " d, tblitems i WHERE inv.invoicenumber = d.invoiceid AND inv.customerid ="+cid+" and "
                            " d.itemid= i.itemid and i.subcatid='"+subcatid+"' ";
            if(begindate!="" && enddate!="")
            {
                query2 += " and inv.invoicedate  >='"+begindate+"' and inv.invoicedate  <='"+enddate+"' ";
            }
            query2 += " group by d.itemid, d.packingUnit order by i.description asc ";
            qDebug()<<query2<<"";
            QSqlQuery info2(query2);
            float total_amt = 0.00;
            while(info2.next())
            {
                QString itemid = info2.value("itemid").toString();
                QString unitname = erp.unitName( info2.value("unitid").toString() );
                QString query_ret="select sum(rd.itemqtyorder) AS totalQtyReturn,sum(rd.itemqtyorder*rd.itempriceach) "
                                  " AS totalReturn from  tblreturn r, tblreturndetail rd WHERE r.id=rd.orderid and "
                                  " r.customer_id='"+cid+"' and rd.itemid='"+itemid+"'";
                if(begindate!="" && enddate!="")
                {
                    query_ret +=" and r.retdate >='"+begindate+"' and r.retdate <='"+enddate+"'";
                }
                query_ret +=" group by r.customer_id and rd.itemid ";
                QSqlQuery info_ret(query_ret);
                info_ret.next();
                float avgprc=0.00;
                int net_Qty = info2.value("tqty").toInt() - info_ret.value("totalQtyReturn").toInt();
                float net_amount = info2.value("tprice").toFloat()-info_ret.value("totalReturn").toFloat();
                if(net_Qty>0)
                {
                    avgprc = net_amount/net_Qty;
                }
                float net_avg_amount = (net_Qty*avgprc);
                html+="<tr>"
                          "<td align='center'>"+info2.value("itemcode").toString()+"</td>"
                          "<td>"+info2.value("description").toString()+"</td>"
                          "<td align='center'>"+ unitname +"</td>"
                          "<td align='center'>"+ erp.amountString(net_Qty) +"</td>"
                          "<td align='right'>"+ erp.amountString(avgprc) +"</td>"
                          "<td align='right'>"+ erp.amountString(net_avg_amount) +"</td>"
                      "</tr>";
                total_amt +=net_avg_amount;
            }

            html += "<tr>"
                    "<th colspan='5' align='right'>"+subcatname+"'s Total:</th>"
                    "<th align='right'>"+ erp.amountString(total_amt) +"</th>"
                    "</tr>";
            total_value +=total_amt;
        }
        totaleprice+=total_value;
        tot_discount = info1.value("discount").toFloat()+info1.value("extraDiscount").toFloat();
        net_total=total_value+info1.value("freight").toFloat()-info1.value("discount").toFloat()-info1.value("extraDiscount").toFloat();
        html += "<tr>"
                "<th colspan='4' rowspan='4'></th>"
                "<th align='right'>Total:</th>"
                "<th align='right'>"+ erp.amountString(total_value) +"</th>"
                "</tr>";
        html += "<tr>"
                "<th align='right'>Freight:</th>"
                "<th align='right'>"+ erp.amountString( info1.value("freight").toFloat() ) +"</th>"
                "</tr>";
        html += "<tr>"
                "<th align='right'>Discount:</th>"
                "<th align='right'>"+ erp.amountString( tot_discount ) +"</th>"
                "</tr>";
        html += "<tr>"
                "<th align='right'>Net Total:</th>"
                "<th align='right'>"+ erp.amountString( net_total ) +"</th>"
                "</tr>";
    }


    html +="</table>";

/******************************************* Header****************************************************/

    QString header_html = "<table width='100%'>"
                          "<tr>"
                          "<td rowspan='2' width='20%' valign='bottom'>Print Date: "+QDate::currentDate().toString("dd/MM/yyyy")+"</td>"
                          "<td width='60%' align='center' style='font-size:22px;text-transform:uppercase;'><b>"+erp.companyname()+"</b></td>"
                          "<td rowspan='2' width='20%' align='right' valign='bottom'></td>"
                          "</tr>"
                          "<tr>"
                          "<th style='font-size:16px;'>Customer's Item Wise Sale</th>"
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
