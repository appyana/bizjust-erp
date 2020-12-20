#include "customerwiseprofitanalysis.h"
#include "ui_customerwiseprofitanalysis.h"

CustomerWiseProfitAnalysis::CustomerWiseProfitAnalysis(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CustomerWiseProfitAnalysis)
{
    ui->setupUi(this);
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(loadParams()) );
    timer->start(5);
}

CustomerWiseProfitAnalysis::~CustomerWiseProfitAnalysis()
{
    delete ui;
}


void CustomerWiseProfitAnalysis::autocompleter(QString sql, QLineEdit *name_txt, QLineEdit *id_txt)
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

void CustomerWiseProfitAnalysis::onItemHighlighted(const QModelIndex &index)
{
    QString code = index.data(Qt::UserRole).toString();
    QString sname = index.data(0).toString();
    sch.searchname = sname;
    sch.searchid = code;
    sch.id_txt->setText(code);
}
void CustomerWiseProfitAnalysis::editingFinished()
{
    QString sname = sch.name_txt->text();
    QString sid = sch.id_txt->text();
    if(sname!=sch.searchname || sid != sch.searchid)
    {
        sch.name_txt->setText("");
        sch.id_txt->setText("");
    }
}

void CustomerWiseProfitAnalysis::loadParams()
{
    timer->stop();
    ui->begindate->setDate( erp.reportBeginDate() );
    ui->enddate->setDate( QDate::currentDate() );
}

void CustomerWiseProfitAnalysis::on_customername_textEdited(const QString &arg1)
{
    autocompleter(sch.customers(arg1),ui->customername,ui->customerid);
}

void CustomerWiseProfitAnalysis::on_btn_next_clicked()
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

void CustomerWiseProfitAnalysis::printPreview(QPrinter *p)
{
    QString begindate = ui->begindate->date().toString("yyyy-MM-dd");
    QString enddate = ui->enddate->date().toString("yyyy-MM-dd");
    QString customerid = ui->customerid->text();
/******************************************report detail***********************************************/
    html = "";
    QString query = "";
    if(customerid=="")
    {
        query = "select tblcustomers.name,tblcustomers.customerid,tblinvoice.* from tblcustomers "
                        " inner join tblinvoice on (tblcustomers.customerid=tblinvoice.customerid) "
                        " and tblinvoice.invoicedate>='"+begindate+"' "
                        "and tblinvoice.invoicedate<='"+enddate+"' order by tblcustomers.name ";
    }
    else
    {
        query = "select tblcustomers.name,tblcustomers.customerid,tblinvoice.* from tblcustomers "
                        " inner join tblinvoice on (tblcustomers.customerid=tblinvoice.customerid) "
                        " WHERE tblcustomers.customerid = '"+customerid+"' "
                        " and tblinvoice.invoicedate>='"+begindate+"' "
                        "and tblinvoice.invoicedate<='"+enddate+"' order by tblcustomers.name ";
    }

    html += "<table width='100%' border='1' cellspacing='0' cellpadding='3'>"
            "<thead>"
            "<tr bgcolor='#f2f2f2'>"
            "<th colspan='5'>From: "+ erp.DDMMYYDateFromSQL(begindate) +" To: "+ erp.DDMMYYDateFromSQL(enddate) +"</th>"
            "</tr>";

    html +="<tr bgcolor='#f2f2f2'>"
               "<th width='40%'>Customer Name</th>"
               "<th width='15%'>Invoice Number</th>"
               "<th width='15%'>Invoice Date</th>"
               "<th width='15%'>Sale(Amount)</th>"
               "<th width='15%'>Profit</th>"
           "</tr></thead>";
    float total = 0;
    float totalprofit = 0;
    QSqlQuery info(query);
    while(info.next())
    {
        QString num = info.value("invoicenumber").toString();
        QString customerid = info.value("customerid").toString();
        QString customername = erp.customerName(customerid);
        QString invoicedate = info.value("invoicedate").toDate().toString("dd/MM/yyyy");
        float net_total = info.value("net_total").toFloat();

        QString query_m="select tblinvoicedetail.*,tblitems.categoryid,(select name from tblitemcategory where "
                        " id=tblitems.categoryid) as catname from tblinvoicedetail left join tblitems on "
                        " tblinvoicedetail.itemid = tblitems.itemid where invoiceid='"+num+"' group by tblitems.itemid";
        QSqlQuery info_m(query_m);
        float profit=0;
        float totprofit=0;
        while(info_m.next())
        {
            float itemcost = erp.getItemField(info_m.value("itemid").toString(),"itemcosteach").toFloat();
            profit = (info_m.value("priceach").toFloat() * info_m.value("qty").toInt() )-(itemcost * info_m.value("qty").toInt() );
            totprofit += profit;
        }

        html +="<tr>"
                   "<td>"+customername+"</td>"
                   "<td align='center'>"+num+"</td>"
                   "<td align='center'>"+invoicedate+"</td>"
                   "<td align='right'>"+ erp.amountString(net_total) +"</td>"
                   "<td align='right'>"+ erp.amountString(totprofit) +"</td>"
               "</tr>";
        total += net_total;
        totalprofit += totprofit;
    }

    html += "<tr>"
            "<th colspan='3' align='right'>Total:</th>"
            "<th align='right'>"+ erp.amountString(total) +"</th>"
            "<th align='right'>"+ erp.amountString(totalprofit) +"</th>"
            "</tr>";
    float avg;
    if(total!=0)
    {
        avg=(totalprofit/total)*100;
    }
    else
    {
        avg=(totalprofit/1)*100;
    }

    html += "<tr>"
            "<th colspan='4' align='right'>Average Profit Ratio:</th>"
            "<th align='right'>"+ erp.amountString(avg) +"</th>"
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
                          "<th style='font-size:16px;'>Customer Wise Profit Analysis</th>"
                          "</tr>"
                          "<!--<tr><td colspan='3'><hr/></td></tr>-->"
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
