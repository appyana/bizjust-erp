#include "accountsbalancesummary.h"
#include "ui_accountsbalancesummary.h"

AccountsBalanceSummary::AccountsBalanceSummary(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AccountsBalanceSummary)
{
    ui->setupUi(this);
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(loadParams()) );
    timer->start(5);
}

AccountsBalanceSummary::~AccountsBalanceSummary()
{
    delete ui;
}


void AccountsBalanceSummary::loadParams()
{
    timer->stop();
    ui->duedate->setDate( QDate::currentDate() );
    QSqlQuery qry("select * from tblaccounttype order by id asc");
    ui->accgroup->addItem("All",QVariant(-1));
    while(qry.next())
    {
        ui->accgroup->addItem(qry.value("description").toString(),qry.value("id"));
    }
    on_accgroup_currentIndexChanged(0);
}


void AccountsBalanceSummary::on_accgroup_currentIndexChanged(int index)
{
    ui->accgroup_sub->clear();
    QString accgroup = ui->accgroup->itemData(index).toString();
    ui->accgroup_sub->addItem("All",QVariant(-1));
    QSqlQuery qry("select * from tblglaccount where accountypeid='"+accgroup+"' and grouptype='group' ");
    while(qry.next())
    {
        ui->accgroup_sub->addItem(qry.value("description").toString(),qry.value("id"));
    }
}


void AccountsBalanceSummary::on_btn_next_clicked()
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
    pd->setWindowTitle("Account Balances Summary");
    pd->setWindowFlags(Qt::Window);
    pd->resize(900,600);
    pd->show();
}

void AccountsBalanceSummary::printPreview(QPrinter *p)
{
    QString duedate = ui->duedate->date().toString("yyyy-MM-dd");
    QString actualDate = duedate;
    QString accgroup = ui->accgroup->itemData(ui->accgroup->currentIndex()).toString();
    QString accgroup_sub = ui->accgroup_sub->itemData(ui->accgroup_sub->currentIndex()).toString();
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
    float grand_total_debit=0;
    float grand_total_credit=0;
    QString query="select distinct(t.id), t.description from tblaccounttype t, tblglaccount a, tblgltransaction s where t.id=a.accountypeid and s.glaccountid=a.id order by id";
    //qDebug()<<query_2<<endl;

    QSqlQuery info(query);
    while(info.next())
    {
        float act_type_total=0;
        float total_debit=0;
        float total_credit=0;
        QString acct_typeid = info.value("id").toString();
        //QString query_act;
        QString query_act = " select a.id, a.description, tr.status, tr.entrydate, a.name, a.summaryaccountid, sum(t.debit) as "
                            " debit, sum(t.credit) as credit, a.accountypeid from tblglaccount a, tblgltransaction t , "
                            " tblgltransvoucher tr where a.id = t.glaccountid and t.voucherno=tr.voucherno and "
                            " t.voucherid=tr.voucherid and accountypeid ='"+acct_typeid+"'  and tr.entrydate <= '"+actualDate+"' ";
        if(accgroup.toInt() != -1)
        {
            query_act += " and t.glaccountid IN (select ID from tblglaccount where accountypeid='"+accgroup+"') " ;
        }
        if(accgroup_sub.toInt() != -1)
        {
            query_act ="select a.id, a.description, tr.status, tr.entrydate, a.name, a.summaryaccountid, sum(t.debit) as "
                   " debit, sum(t.credit) as credit, a.accountypeid from tblglaccount a, tblgltransaction t , "
                   " tblgltransvoucher tr where a.id = t.glaccountid and t.voucherno=tr.voucherno and "
                   " t.voucherid=tr.voucherid and tr.entrydate <= '"+actualDate+"' "
                   " and accountypeid ='"+acct_typeid+"' and t.glaccountid IN (select ID from tblglaccount where "
                   " accountypeid='"+accgroup+"') and a.summaryaccountid='"+accgroup_sub+"' and a.grouptype='detail' " ;
        }
        query_act +=" group by t.glaccountid ";
        //qDebug()<<query<<endl;

        html += "<tr bgcolor='#f2f2f2'>"
                "<th colspan='6'>"+info.value("description").toString()+"</th>"
                "</tr>";
        html += "<tr bgcolor='#f2f2f2'>"
                "<th>Account Description</th>"
                "<th>Account Number</th>"
                "<th>Summarize To</th>"
                "<th>Account Type</th>"
                "<th>Debit</th>"
                "<th>Credit</th>"
                "</tr>";
        QSqlQuery row(query_act);
        while(row.next())
        {
            QString id = row.value("id").toString();
            float t_debit=0;
            float t_credit=0;
            int flag=0;
            float netbalance=0;
            //float t_beg_debit=0;
            //float t_beg_credit=0;
            QString accountypeid = row.value("accountypeid").toString();
            QString query5="select sum(r.debit) as debit, sum(r.credit) as credit from tblgltransvoucher v, tblgltransaction r, tblglaccount a where  r.glaccountid = a.id and r.voucherno = v.voucherno and r.voucherid = v.voucherid and v.entrydate <= '"+actualDate+"' and a.id = '"+id+"' ";
            if( accountypeid == "50" || accountypeid=="70" || accountypeid=="80" || accountypeid=="90" )
            {
                query5 = "select sum(r.debit) as debit, sum(r.credit) as credit from tblgltransvoucher v, tblgltransaction r, tblglaccount a where  r.glaccountid = a.id and r.voucherno = v.voucherno and r.voucherid = v.voucherid and v.entrydate <= '"+actualDate+"' and a.id = '"+id+"' ";
            }
            //qDebug()<<query5<<endl;
            QSqlQuery row5(query5);
            row5.next();
//            QSqlQuery info_ob("select balance from tblglopeningbalance where glaccountid='"+id+"' and financialyearid='"+financialyearid+"' ");
//            info_ob.next();
            t_debit = row5.value("debit").toFloat();
            t_credit = row5.value("credit").toFloat();
//                float ob_bal = info_ob.value("balance").toFloat();
//                if(ob_bal>0)
//                {
//                    t_debit += ob_bal;
//                }
//                else
//                {
//                    t_credit += ob_bal;
//                }
            if( t_debit >= t_credit )
            {
                flag = 1 ;
                netbalance = t_debit - t_credit;
            }
            else
            {
                flag = 2 ;
                netbalance = t_credit - t_debit;
            }
            act_type_total =   netbalance - act_type_total;
            //if(netbalance!=0)
            //{
            QString summaryaccountid= row.value("summaryaccountid").toString();
                QSqlQuery row3(" select * from tblglaccount  where id = '"+ summaryaccountid +"' ");
                row3.next();
                html+="<tr>"
                      "<td>"+row.value("description").toString()+"</td>"
                      "<td align='center'>"+row.value("name").toString()+"</td>"
                      "<td>"+row3.value("description").toString()+"</td>"
                      "<td align='center'>"+erp.accountField(row.value("id").toString(),"grouptype").toUpper()+"</td>"
                      "<td align='right'>";
                if(flag==1)
                {
                    html += erp.amountString(netbalance);
                    total_debit += netbalance;
                }
                html+="</td>"
                      "<td align='right'>";
                if(flag==2)
                {
                    html += erp.amountString(netbalance);
                    total_credit += netbalance;
                }
                html+="</td>"
                      "</tr>";
            //}
        }

        if(accgroup=="10" || accgroup=="11" || accgroup=="13" || accgroup=="80" || accgroup=="90")
        {
            act_type_total = total_debit - total_credit ;
        }
        else if(accgroup=="21" || accgroup=="22" || accgroup=="23" || accgroup=="30" || accgroup=="40" || accgroup=="50" || accgroup=="60" || accgroup=="70")
        {
            act_type_total = total_credit - total_debit;
        }

        html+="<tr>"
              "<th colspan='4' align='right'>Total "+info.value("description").toString()+":</th>"
              "<th align='right'>"+erp.amountString(total_debit)+"</th>"
              "<th align='right'>"+erp.amountString(total_credit)+"</th>"
              "</tr>";
        grand_total = grand_total + act_type_total;
        grand_total_debit += total_debit;
        grand_total_credit += total_credit;
    }

    html+="<tr>"
          "<th colspan='4' align='right'>Grand Total:</th>"
          "<th align='right'>"+erp.amountString(grand_total_debit)+"</th>"
          "<th align='right'>"+erp.amountString(grand_total_credit)+"</th>"
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
                          "<th style='font-size:16px;'>Account Balances Summary</th>"
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
