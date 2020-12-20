#include "customersalesanalysis.h"
#include "ui_customersalesanalysis.h"

CustomerSalesAnalysis::CustomerSalesAnalysis(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CustomerSalesAnalysis)
{
    ui->setupUi(this);
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(loadParams()) );
    timer->start(5);
}

CustomerSalesAnalysis::~CustomerSalesAnalysis()
{
    delete ui;
}


void CustomerSalesAnalysis::loadParams()
{
    timer->stop();
    ui->begindate->setDate( erp.reportBeginDate() );
    ui->enddate->setDate( QDate::currentDate() );
}

void CustomerSalesAnalysis::on_btn_next_clicked()
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
    pd->setWindowTitle("Customer Sales Analysis");
    pd->setWindowFlags(Qt::Window);
    pd->resize(900,600);
    pd->show();
}

void CustomerSalesAnalysis::printPreview(QPrinter *p)
{
    QString begindate = ui->begindate->date().toString("yyyy-MM-dd");
    QString enddate = ui->enddate->date().toString("yyyy-MM-dd");

/******************************************report detail***********************************************/
    html = "";

    html += "<table width='100%' border='1' cellspacing='0' cellpadding='3'>"
            "<thead><tr bgcolor='#f2f2f2'>"
            "<th colspan='4'>From: "+ erp.DDMMYYDateFromSQL(begindate) +" To: "+ erp.DDMMYYDateFromSQL(enddate) +"</th>"
            "</tr>";

    html +="<tr bgcolor='#f2f2f2'>"
               "<th width='10%'>Sr #.</th>"
               "<th width='65%'>Customer Name</th>"
               "<th width='15%'>Total Sale</th>"
               "<th width='10%'>Percentage</th>"
           "</tr></thead>";

    QString query_tot="select sum(id.qty*id.priceach) as NetAmount,sum(i.freight) as fri, sum(i.totalDiscount) as discount, sum(i.extraDiscount) as extraDiscount from tblinvoice i, tblinvoicedetail id where i.invoicenumber=id.invoiceid";
    if(begindate!="" && enddate!="")
    {
        query_tot += " and  i.invoicedate >= '"+begindate+"' and i.invoicedate <= '"+enddate+"' ";
    }
    QSqlQuery info_tot(query_tot);
    info_tot.next();

    QString query_ret="select sum(rd.itemqtyorder*rd.itempriceach) AS totalReturn from  tblreturn r, tblreturndetail rd WHERE r.id=rd.orderid ";
    if(begindate!="" && enddate!="")
    {
        query_ret += " and  r.retdate >= '"+begindate+"' and r.retdate <= '"+enddate+"' ";
    }
    QSqlQuery info_ret(query_ret);
    info_ret.next();

    float Tot_Sales=0;
    QString queryt = "select i.customerid,c.name,i.invoicedate,sum(id.qty*id.priceach) as totalAmount,sum(i.freight) as fri, "
                     " sum(i.totalDiscount) as discount, sum(i.extraDiscount) as extraDiscount from tblinvoice i, "
                     " tblinvoicedetail id , tblcustomers c where i.invoicenumber=id.invoiceid and "
                     " i.customerid=c.customerid ";
    if(begindate!="" && enddate!="")
    {
        queryt += " and  i.invoicedate >= '"+begindate+"' and i.invoicedate <= '"+enddate+"' ";
    }
    queryt += " group by i.customerid ORDER BY c.name asc ";
    //qDebug()<<query_2<<endl;
    int i=1;
    QSqlQuery infot(queryt);
    while(infot.next())
    {
        QString customerid = infot.value("customerid").toString();
        QString customername = infot.value("name").toString();
        float ReturnSales = erp.CustomerReturnSalesDated(customerid,begindate,enddate);
        float Total = infot.value("totalAmount").toFloat()+infot.value("fri").toFloat()-infot.value("discount").toFloat()-infot.value("extraDiscount").toFloat()-ReturnSales;
        float NetTotal = info_tot.value("NetAmount").toFloat()+info_tot.value("fri").toFloat()-info_tot.value("discount").toFloat()-info_tot.value("extraDiscount").toFloat()-info_ret.value("totalReturn").toFloat();
        float sale_percentage = (Total/NetTotal) * 100;

        html+="<tr>"
              "<td align='center'>"+ erp.intString(i) +"</td>"
              "<td>"+ customername +"</td>"
              "<td align='right'>"+erp.amountString(Total)+"</td>"
              "<td align='center'>"+QString::number(sale_percentage,'f',2)+"%</td>"
              "</tr>";
        i++;
        Tot_Sales       +=Total;

    }

    html+="<tr>"
          "<th align='right' colspan='2'>Totals:</th>"
          "<th align='right'>"+erp.amountString(Tot_Sales)+"</th>"
          "<th align='right'></th>"
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
                          "<th style='font-size:16px;'>Customer Sales Analysis</th>"
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
