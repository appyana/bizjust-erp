#include "profitandloss.h"
#include "ui_profitandloss.h"

ProfitAndLoss::ProfitAndLoss(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProfitAndLoss)
{
    ui->setupUi(this);
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(loadParams()) );
    timer->start(5);
}

ProfitAndLoss::~ProfitAndLoss()
{
    delete ui;
}


void ProfitAndLoss::loadParams()
{
    timer->stop();
    ui->begindate->setDate( erp.reportBeginDate() );
    ui->enddate->setDate( QDate::currentDate() );
}


void ProfitAndLoss::on_btn_next_clicked()
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
    pd->setWindowTitle("Profit and Loss");
    pd->setWindowFlags(Qt::Window);
    pd->resize(900,600);
    pd->show();
}

void ProfitAndLoss::printPreview(QPrinter *p)
{
    QString begindate = ui->begindate->date().toString("yyyy-MM-dd");
    QString enddate = ui->enddate->date().toString("yyyy-MM-dd");
    //bool zero_bal=true;
    //QString financialyearid = erp._yearid;

/******************************************report detail***********************************************/
    html = "";

    html += "<table width='100%' border='1' cellspacing='0' cellpadding='3'>";
    html += "<tr bgcolor='#f2f2f2'>"
            "<th width='60%'>Account Description - Account</th>"
            "<th width='20%'>Period</th>"
            "<th width='20%'>YTD</th>"
            "</tr>";

    float grand_sales = 0 ;
    float grand_expenses = 0 ;
    float grand_costs=0;
    float grand_sales_p=0;
    float grand_expenses_p=0;
    float grand_costs_p=0;
    float cosOfGoodTotal_p  = 0;
    float cosOfGoodTotal_Ytd= 0;

    QString query = "SELECT id,description FROM tblaccounttype WHERE id IN ('50','70','80','90') ORDER BY id";
    QSqlQuery info(query);
    while(info.next())
    {
        float act_type_total = 0 ;
        float act_type_total_p = 0 ;
        float sales = 0 ;
        float sales_p = 0 ;
        float expenses = 0;
        float expenses_p = 0;
        float costs = 0 ;
        float costs_p = 0 ;
        float openingStock=0;
        float YtdOpeningStock=0;
        QString YEAR = QDate::currentDate().toString("yyyy");
        QString YTD = YEAR+"-01-01";
        QString infoid = info.value("id").toString();
        QString infodescription = info.value("description").toString();
        QString query1 = "select a.id, a.description, a.name, sum(t.debit) as debit, sum(t.credit) as credit from tblglaccount a, tblgltransaction t , tblgltransvoucher tr where a.id = t.glaccountid and t.voucherno=tr.voucherno and tr.entrydate >= '"+YTD+"' and tr.entrydate <= '"+enddate+"' and accountypeid ='"+infoid+"' group by t.glaccountid" ;
        if(infodescription == "Cost of Goods Sold")
        {
            html +="<tr bgcolor='#f2f2f2'>"
                   "<th colspan='3'>"+infodescription+"</th>"
                   "</tr>";
            QString opstockdate = ui->begindate->date().addDays(-1).toString("yyyy-MM-dd");
            //QString ytdopstockdate = erp.SQLDateToQtDate(YTD).addDays(-1).toString("yyyy-MM-dd");
            openingStock=erp.calculateStockWeighted(opstockdate);
            QSqlQuery info_inventory("select inventory from tblinvcompany where id='1' ");
            info_inventory.next();
            QString glacct = info_inventory.value("inventory").toString();
            QSqlQuery info_ytdStock("select sum(debit-credit) as Ytd_OS from tblgltransaction where glaccountid='"+glacct+"'");
            info_ytdStock.next();
            YtdOpeningStock = info_ytdStock.value("Ytd_OS").toFloat();
            html +="<tr>"
                   "<th align='right'>Opening Stock:</th>"
                   "<th align='right'>"+erp.amountString(openingStock)+"</th>"
                   "<th align='right'>"+erp.amountString(YtdOpeningStock)+"</th>"
                   "</tr>";
        }
        else
        {
            html +="<tr bgcolor='#f2f2f2'>"
                   "<th colspan='3'>"+infodescription+"</th>"
                   "</tr>";
        }
        QSqlQuery row(query1);
        while(row.next())
        {
            QString rowid = row.value("id").toString();
            QString query2 = "select a.id as `id`, a.description as `description`, tr.status as `status`, tr.entrydate as `entrydate`, a.name as `name`, sum(t.debit) as `debit`, sum(t.credit) as `credit` from tblglaccount a, tblgltransaction t, tblgltransvoucher tr where a.id=t.glaccountid and t.voucherno=tr.voucherno and tr.entrydate >= '"+begindate+"' and tr.entrydate <= '"+enddate+"' and accountypeid='"+infoid+"' and t.glaccountid='"+rowid+"' group by t.glaccountid ";
            QSqlQuery rw(query2);
            rw.next();
            //float t_debit = 0 ;
            //float t_credit = 0 ;
            float netbalance = 0;
            float netbalance_p = 0;
            if( infoid=="70")
            {
                netbalance = row.value("debit").toFloat() - row.value("credit").toFloat();
                netbalance_p = rw.value("debit").toFloat() - rw.value("credit").toFloat();

                act_type_total = act_type_total + netbalance;
                act_type_total_p = act_type_total_p + netbalance_p;

                costs = costs + netbalance;
                costs_p = costs_p + netbalance_p;
            }
            else if(infoid=="80" || infoid=="90")
            {
                netbalance = row.value("debit").toFloat() - row.value("credit").toFloat();
                netbalance_p = rw.value("debit").toFloat() - rw.value("credit").toFloat();

                act_type_total = act_type_total + netbalance;
                act_type_total_p = act_type_total_p + netbalance_p;

                expenses = expenses + netbalance;
                expenses_p = expenses_p + netbalance_p;
            }
            else if(infoid=="50")
            {
                netbalance = row.value("credit").toFloat() - row.value("debit").toFloat();
                netbalance_p = rw.value("credit").toFloat() - rw.value("debit").toFloat();

                act_type_total = act_type_total + netbalance;
                act_type_total_p = act_type_total_p + netbalance_p;

                sales = sales + netbalance;
                sales_p = sales_p + netbalance_p;
            }
            html +="<tr>"
                   "<td>"+row.value("description").toString()+" ("+row.value("name").toString()+")</td>"
                   "<td align='right'>"+erp.amountString(netbalance_p)+"</td>"
                   "<td align='right'>"+erp.amountString(netbalance)+"</td>"
                   "</tr>";
        }
        if(infodescription=="Cost of Goods Sold")
        {
            html +="<tr>"
                   "<th align='right'>Total Purchases:</th>"
                   "<th align='right'>"+erp.amountString(act_type_total_p)+"</th>"
                   "<th align='right'>"+erp.amountString(act_type_total)+"</th>"
                   "</tr>";
            float closingStock=erp.calculateStockWeighted(enddate);
            html +="<tr>"
                   "<th align='right'>Closing Stock:</th>"
                   "<th align='right'>"+erp.amountString(closingStock)+"</th>"
                   "<th align='right'>"+erp.amountString(closingStock)+"</th>"
                   "</tr>";
            cosOfGoodTotal_p  = openingStock + act_type_total_p - closingStock;
            cosOfGoodTotal_Ytd= YtdOpeningStock + act_type_total - closingStock;
            html +="<tr>"
                   "<th align='right'>Total:</th>"
                   "<th align='right'>"+erp.amountString(cosOfGoodTotal_p)+"</th>"
                   "<th align='right'>"+erp.amountString(cosOfGoodTotal_Ytd)+"</th>"
                   "</tr>";
        }
        else
        {
            html +="<tr>"
                   "<th align='right'>Total:</th>"
                   "<th align='right'>"+erp.amountString(act_type_total_p)+"</th>"
                   "<th align='right'>"+erp.amountString(act_type_total)+"</th>"
                   "</tr>";
        }
        grand_sales = grand_sales + sales ;
        grand_sales_p = grand_sales_p + sales_p ;

        grand_expenses = grand_expenses + expenses ;
        grand_expenses_p = grand_expenses_p + expenses_p ;

        grand_costs = grand_costs + costs ;
        grand_costs_p = grand_costs_p + costs_p ;
    }

    float grand_total = 	grand_sales - grand_expenses - cosOfGoodTotal_Ytd ;
    float grand_total_p = grand_sales_p - grand_expenses_p - cosOfGoodTotal_p ;

    html += "<tr>"
            "<th align='right'>Grand Total:</th>"
            "<th align='right'>"+erp.amountString(grand_total_p)+"</th>"
            "<th align='right'>"+erp.amountString(grand_total)+"</th>"
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
                          "<th style='font-size:16px;'>Profit and Loss</th>"
                          "</tr>"
                          "<tr><th colspan='3'>From: "+erp.DDMMYYDateFromSQL(begindate)+" To: "+erp.DDMMYYDateFromSQL(enddate)+"</th></tr>"
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
