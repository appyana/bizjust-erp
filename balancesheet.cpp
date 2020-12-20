#include "balancesheet.h"
#include "ui_balancesheet.h"

BalanceSheet::BalanceSheet(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BalanceSheet)
{
    ui->setupUi(this);
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(loadParams()) );
    timer->start(5);
}

BalanceSheet::~BalanceSheet()
{
    delete ui;
}


void BalanceSheet::loadParams()
{
    timer->stop();
    ui->duedate->setDate( QDate::currentDate() );
    QSqlQuery qry("select * from tblaccounttype order by id asc");
}


void BalanceSheet::on_btn_next_clicked()
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
    pd->setWindowTitle("Balance Sheet");
    pd->setWindowFlags(Qt::Window);
    pd->resize(900,600);
    pd->show();
}

void BalanceSheet::printPreview(QPrinter *p)
{
    QString duedate = ui->duedate->date().toString("yyyy-MM-dd");
    QString actualDate = duedate;
    //bool zero_bal=true;
    //QString financialyearid = erp._yearid;

/******************************************report detail***********************************************/
    html = "";

    html += "<table width='100%' border='1' cellspacing='0' cellpadding='3'>";
//    QSqlQuery info_f("select * from tblfinancialyear where yearid='"+financialyearid+"'");
//    info_f.next();
//    QString begindate = info_f.value("startdate").toString();
    //QString show="0";
    float grand_total=0;
    float grand_total4_accType=0;
    QString query = "select accT.basicatid accID,sum(t.debit) as debit, sum(t.credit) as credit from tblaccounttype accT, tblglaccount a, tblgltransaction t , tblgltransvoucher tr where a.id = t.glaccountid and t.voucherno=tr.voucherno and tr.entrydate <= '"+actualDate+"' and accT.basicatid in (1) group by accT.basicatid";
    //qDebug()<<query_2<<endl;
    float total_assets=0;
    float total_liability=0;
    float total_capital=0;
    float closingStock=0;
    QSqlQuery row_assets(query);
    while (row_assets.next())
    {
        float debit = row_assets.value("debit").toFloat();
        float credit = row_assets.value("credit").toFloat();
        QString accID = row_assets.value("accID").toString();
        if(accID=="1")
        {
            total_assets = debit-credit;
        }
        else if(accID=="2")
        {
            total_liability = credit-debit;
        }
        else if(accID=="3")
        {
            total_capital = credit-debit;
        }
    }
    query = "select distinct(t.id), t.description from tblaccounttype t, tblglaccount a, tblgltransaction s where t.id=a.accountypeid and s.glaccountid=a.id AND t.basicatid in(1) order by id";
    QSqlQuery info(query);
    html += "<tr bgcolor='#f2f2f2'>"
            "<th width='60%'>Account Description</th>"
            "<th width='20%'>Account Number</th>"
            "<th width='20%'>Account Balance</th>"
            "</tr>"
            "<tr bgcolor='#f2f2f2'>"
            "<th colspan='3'>ASSETS</th>"
            "</tr>";
    while (info.next())
    {
        float act_type_total = 0 ;
        QSqlQuery info_inv("select inventory, costadjustglacctid from tblinvcompany where id='1'");
        QString acct_inv_id = info_inv.value("inventory").toString();
        QString stock_adjustment_id = info_inv.value("costadjustglacctid").toString();
        QString infoid = info.value("id").toString();
        query = "select a.id, a.description, tr.status, tr.entrydate, a.name, sum(t.debit) as debit, sum(t.credit) as credit from tblglaccount a, tblgltransaction t , tblgltransvoucher tr where a.id = t.glaccountid and t.voucherno=tr.voucherno and tr.entrydate <= '"+actualDate+"' and accountypeid ='"+infoid+"' and a.id not in('"+acct_inv_id+"','"+stock_adjustment_id+"')  group by t.glaccountid ";
        QSqlQuery row(query);
        html += "<tr bgcolor='#f2f2f2'>"
                "<th colspan='3' align='left'>"+info.value("description").toString()+"</th>"
                "</tr>";
        while(row.next())
        {
            float netbalance = 0;
            if( infoid=="10" || infoid=="11" || infoid=="13" || infoid=="80" || infoid=="90" )
            {
                netbalance = row.value("debit").toFloat() - row.value("credit").toFloat();
                act_type_total = act_type_total + netbalance;
            }
            else if( infoid=="21" || infoid=="22" || infoid=="23" || infoid=="30"  || infoid=="50" || infoid=="60" || infoid=="70")
            {
                netbalance = row.value("credit").toFloat() - row.value("debit").toFloat();
                act_type_total = act_type_total + netbalance;
            }
            if(netbalance!=0)
            {
                html+="<tr>"
                      "<td>"+row.value("description").toString()+"</td>"
                      "<td align='center'>"+row.value("name").toString()+"</td>"
                      "<td align='right'>"+erp.amountString(netbalance)+"</td>"
                      "</tr>";
            }
        }// end inner while loop
        if(info.value("description").toString()=="Current Assets")
        {
            closingStock = erp.calculateStockWeighted(actualDate);
            html +="<tr>"
                   "<th colspan='2' align='right'>Closing Stock:</th>"
                   "<th align='right'>"+erp.amountString(closingStock)+"</th>"
                   "</tr>";
            html +="<tr>"
                   "<th colspan='2' align='right'>Total "+info.value("description").toString()+":</th>"
                   "<th align='right'>"+erp.amountString((act_type_total+closingStock))+"</th>"
                   "</tr>";
        }
        else
        {
            html +="<tr>"
                   "<th colspan='2' align='right'>Total "+info.value("description").toString()+":</th>"
                   "<th align='right'>"+erp.amountString((act_type_total))+"</th>"
                   "</tr>";
        }
        grand_total4_accType += act_type_total;
        grand_total = grand_total + act_type_total;
    }// end outer while loop
    if(grand_total4_accType>0)
    {
        html +="<tr>"
               "<th colspan='2' align='right'>Total Assets:</th>"
               "<th align='right'>"+erp.amountString((grand_total4_accType+closingStock))+"</th>"
               "</tr>";
    }
    html += "<tr bgcolor='#f2f2f2'>"
            "<th colspan='3'>LIABILITIES AND EQUITY</th>"
            "</tr>";
    float grand_totalLiab=0;
    grand_total4_accType=0;

    query = "select distinct(t.id), t.description from tblaccounttype t, tblglaccount a, tblgltransaction s where t.id=a.accountypeid and s.glaccountid=a.id AND t.basicatid in(2) order by id";
    QSqlQuery info2(query);//QSqlQuery
    while(info2.next())
    {
        float act_type_total = 0;
        QString infoid = info2.value("id").toString();
        query = "select a.id, a.description, tr.status, tr.entrydate, a.name, sum(t.debit) as debit, sum(t.credit) as credit from tblglaccount a, tblgltransaction t , tblgltransvoucher tr where a.id = t.glaccountid and t.voucherno=tr.voucherno and tr.entrydate <= '"+actualDate+"' and accountypeid ='"+infoid+"' group by t.glaccountid ";
        QSqlQuery row(query);
        html += "<tr bgcolor='#f2f2f2'>"
                "<th colspan='3' align='left'>"+info2.value("description").toString()+"</th>"
                "</tr>";
        while(row.next())
        {
            float netbalance = 0 ;
            if(infoid=="10" || infoid=="11" || infoid=="13" || infoid=="80" || infoid=="90" )
            {
                netbalance = row.value("debit").toFloat() - row.value("credit").toFloat();
                act_type_total = act_type_total + netbalance ;
            }
            else if(infoid=="21" || infoid=="22" || infoid=="23" || infoid=="30"  || infoid=="50" || infoid=="60" || infoid=="70")
            {
                netbalance = row.value("credit").toFloat() - row.value("debit").toFloat();
                act_type_total = act_type_total + netbalance;
            }
            if(netbalance!=0)
            {
                html+="<tr>"
                      "<td>"+row.value("description").toString()+"</td>"
                      "<td align='center'>"+row.value("name").toString()+"</td>"
                      "<td align='right'>"+erp.amountString(netbalance)+"</td>"
                      "</tr>";
            }
        }// end inner while loop
        html +="<tr>"
               "<th colspan='2' align='right'>Total "+info2.value("description").toString()+":</th>"
               "<th align='right'>"+erp.amountString((act_type_total))+"</th>"
               "</tr>";
        grand_total4_accType += act_type_total;
        grand_total = grand_total + act_type_total;
    }// end outer while loop

    grand_totalLiab +=grand_total4_accType;

    query = "select distinct(t.id), t.description from tblaccounttype t, tblglaccount a, tblgltransaction s where t.id=a.accountypeid and s.glaccountid=a.id AND t.basicatid in(3) order by id ";
    QSqlQuery info3(query);//QSqlQuery
    int nume2 = info3.size();
    int nume_equity = nume2;
    grand_total4_accType = 0;
    float previousearnings=0;
    float grand_totalPrifit=0;
    int aa2 = 0;
    int nume3=0;
    while(info3.next())
    {
        float act_type_total = 0 ;
        aa2 = aa2 + 1;
        nume_equity = nume_equity - 1;
        QString infoid = info3.value("id").toString();
        query = "select a.id, a.description, tr.status, tr.entrydate, a.name, sum(t.debit) as debit, sum(t.credit) as credit from tblglaccount a, tblgltransaction t , tblgltransvoucher tr where a.id = t.glaccountid and t.voucherno=tr.voucherno and tr.entrydate <= '"+actualDate+"' and accountypeid ='"+infoid+"' group by t.glaccountid" ;
        QSqlQuery row(query);
        nume3 = row.size();
        if(aa2==1)
        {
            html += "<tr bgcolor='#f2f2f2'>"
                    "<th colspan='3'>Equity</th>"
                    "</tr>";
        }
        while(row.next())
        {
            float netbalance = 0 ;
            if(infoid=="10" || infoid=="11" || infoid=="13" || infoid=="80" || infoid=="90" )
            {
                netbalance = row.value("debit").toFloat() - row.value("credit").toFloat();
                act_type_total = act_type_total + netbalance ;
            }
            else if( infoid=="21" || infoid=="22" || infoid=="23" || infoid=="30"  || infoid=="50" || infoid=="60" || infoid=="70" || infoid=="40"  )
            {
                netbalance = row.value("credit").toFloat() - row.value("debit").toFloat();
                act_type_total = act_type_total + netbalance;
            }
            if(netbalance!=0)
            {
                html+="<tr>"
                      "<td>"+row.value("description").toString()+"</td>"
                      "<td align='center'>"+row.value("name").toString()+"</td>"
                      "<td align='right'>"+erp.amountString(netbalance)+"</td>"
                      "</tr>";
            }
        }// end inner while loop
        if(nume_equity==0)
        {
            QString YEAR = actualDate.split(QRegExp("/")).value(2);
            QString month = actualDate.split(QRegExp("/")).value(1);
            if(month.toInt()>8)
            {
                YEAR = erp.intString(YEAR.toInt()-1);
            }
            QString bdate = YEAR+"-07-31";
            QSqlQuery info_1styear(" select enddate from tblfinancialyear where yearid='1' ");
            info_1styear.next();
            QString Edate_1styear = info_1styear.value("enddate").toString();
            QDate converted_Edate_1styear = erp.SQLDateToQtDate(Edate_1styear);
            QSqlQuery info_cyear("select yearid from tblfinancialyear where startdate>='"+actualDate+"' and enddate<='"+actualDate+"'");
            int year_id = info_cyear.value("yearid").toInt();
            QString last_yearid = erp.intString(year_id-1);
            QSqlQuery info_year(" select enddate from tblfinancialyear where yearid='"+last_yearid+"' ");
            QString pervious_year = info_year.value("enddate").toString();
            QDate converted_perviousyear = erp.SQLDateToQtDate(pervious_year);
            QDate converted_actualdate = erp.SQLDateToQtDate(actualDate);
            if( ( converted_actualdate > converted_perviousyear ) && ( converted_actualdate > converted_Edate_1styear ))
            {
                previousearnings  = erp.calculateProfitAndLossToDate(bdate);
                html +="<tr>"
                       "<th colspan='2' align='right'>Previous Earnings :</th>"
                       "<th align='right'>"+erp.amountString((previousearnings))+"</th>"
                       "</tr>";
            }
            grand_totalPrifit = erp.calculateProfitAndLossToDate(actualDate);
            html +="<tr>"
                   "<th colspan='2' align='right'>Year-to-date Earnings :</th>"
                   "<th align='right'>"+erp.amountString((grand_totalPrifit))+"</th>"
                   "</tr>";

            html +="<tr>"
                   "<th colspan='2' align='right'>Total Equity:</th>"
                   "<th align='right'>"+erp.amountString((grand_total4_accType+act_type_total+grand_totalPrifit+previousearnings))+"</th>"
                   "</tr>";
        }
        grand_total4_accType += act_type_total;
        grand_total = grand_total + act_type_total;
    }// end outer while loop

    if(grand_total4_accType>0)
    {
        grand_totalLiab += grand_total4_accType;
    }
    if(grand_total4_accType==0 && nume3==0)
    {
        QString YEAR = actualDate.split(QRegExp("/")).value(2);
        QString month = actualDate.split(QRegExp("/")).value(1);
        if(month.toInt()>8)
        {
            YEAR = erp.intString(YEAR.toInt()-1);
        }
        QString bdate = YEAR+"-07-31";
        QSqlQuery info_year(" select enddate from tblfinancialyear order by id asc limit 0,1 ");
        info_year.next();
        QString firstyear = info_year.value("enddate").toString();
        QDate converted_firstyear = erp.SQLDateToQtDate(firstyear);
        QDate converted_actualdate  = erp.SQLDateToQtDate(actualDate);
        if(converted_actualdate > converted_firstyear)
        {
            previousearnings  = erp.calculateProfitAndLossToDate(bdate);
            html +="<tr>"
                   "<th colspan='2' align='right'>Previous Earnings :</th>"
                   "<th align='right'>"+erp.amountString((previousearnings))+"</th>"
                   "</tr>";
        }
    }

    html +="<tr>"
           "<th colspan='2' align='right'>Total Liabilities  and Equity:</th>"
           "<th align='right'>"+erp.amountString((grand_totalLiab+grand_totalPrifit+previousearnings))+"</th>"
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
                          "<th style='font-size:16px;'>Balance Sheet</th>"
                          "</tr>"
                          "<tr><th colspan='3'>For Date: "+erp.DDMMYYDateFromSQL(duedate)+"</th></tr>"
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
