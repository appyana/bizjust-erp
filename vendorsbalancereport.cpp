#include "vendorsbalancereport.h"
#include "ui_vendorsbalancereport.h"

VendorsBalanceReport::VendorsBalanceReport(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VendorsBalanceReport)
{
    ui->setupUi(this);
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(loadParams()) );
    timer->start(5);
}

VendorsBalanceReport::~VendorsBalanceReport()
{
    delete ui;
}


void VendorsBalanceReport::autocompleter(QString sql, QLineEdit *name_txt, QLineEdit *id_txt)
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

void VendorsBalanceReport::onItemHighlighted(const QModelIndex &index)
{
    QString code = index.data(Qt::UserRole).toString();
    QString sname = index.data(0).toString();
    sch.searchname = sname;
    sch.searchid = code;
    sch.id_txt->setText(code);
}
void VendorsBalanceReport::editingFinished()
{
    QString sname = sch.name_txt->text();
    QString sid = sch.id_txt->text();
    if(sname!=sch.searchname || sid != sch.searchid)
    {
        sch.name_txt->setText("");
        sch.id_txt->setText("");
    }
}

void VendorsBalanceReport::loadParams()
{
    timer->stop();
    ui->date->setDate( QDate::currentDate() );
}

void VendorsBalanceReport::on_groupname_textEdited(const QString &arg1)
{
    autocompleter(sch.vendorGroup(arg1),ui->groupname,ui->groupid);
}

void VendorsBalanceReport::on_btn_next_clicked()
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
    pd->setWindowTitle("Vendors Balance Report");
    pd->setWindowFlags(Qt::Window);
    pd->resize(900,600);
    pd->show();
}

void VendorsBalanceReport::printPreview(QPrinter *p)
{
    QString date = ui->date->date().toString("yyyy-MM-dd");
    QString groupid = ui->groupid->text();
    bool showzero = ui->checkBox->isChecked();

/******************************************report detail***********************************************/
    html = "";

    html += "<table width='100%' border='1' cellspacing='0' cellpadding='3'>"
            "<thead>";

    html +="<tr bgcolor='#f2f2f2'>"
               "<th width='20%'>Vendor Group</th>"
               "<th width='10%'>Vendor ID</th>"
               "<th width='40%'>Vendor Name</th>"
               "<th width='15%'>Phone Numbers</th>"
               "<th width='15%'>Balance</th>"
           "</tr></thead>";
    QString query_s= " select ( sum(debit) - sum(credit) ) AS dc, compid, v.vendorname as vname, v.vendorid as "
                     " vendorId, v.phone1, v.phone2, v.phone3 from tblgltransaction t,tblgltransvoucher tv, tblvendors v "
                     " where t.voucherno=tv.voucherno and v.vendorid=t.compid and v.vendorid not in "
                     " (select combineVendorId from tblcustomers where ifnull(combineVendorId,0) != 0) and "
                     " t.companytype='vendor' and tv.entrydate <= '"+date+"' group by compid ";
    if(groupid!="")
    {
        query_s= "select (sum(debit) - sum(credit)) AS dc, compid,  v.vendorname as vname,v.vendorid as vendorId, "
                 " v.phone1, v.phone2, v.phone3 from tblgltransaction t,tblgltransvoucher tv,tblvendors v where "
                 " t.voucherno=tv.voucherno and v.vendorid=t.compid and v.vendorid not in (select combineVendorId "
                 " from tblcustomers where ifnull(combineVendorId,0) != 0) and t.companytype='vendor' and compid "
                 " in(select vendorid from tblvendors where vendor_group_id='"+groupid+"') and "
                 " tv.entrydate <= '"+date+"' group by compid";
    }

    query_s += " order by v.vendorname asc ";
    float total_netopen=0;
    QSqlQuery info_s(query_s);
    while(info_s.next())
    {
        float a_netopen = info_s.value("dc").toFloat();
        QString bal = erp.amountString(a_netopen)+" Dr";
        if(a_netopen<0)
        {
            bal = erp.amountString(a_netopen * -1)+" Cr";
        }
        bool show = true;
        if(showzero==false && a_netopen==0)
        {
            show = false;
        }
        if(show==true)
        {
            QString vendorid = info_s.value("compid").toString();
            QString groupname = erp.getVendorGroupName(erp.vendorField(vendorid,"vendor_group_id"));
            if(erp.vendorField(vendorid,"vendor_group_id").toInt()==0)
            {
                groupname = "No Group";
            }
            QString phones = info_s.value("phone1").toString()+" "+info_s.value("phone2").toString()+" "+info_s.value("phone3").toString();
            html +="<tr>"
                   "<td align='center'>"+groupname+"</td>"
                   "<td align='center'>"+ vendorid +"</td>"
                   "<td>"+erp.vendorname(vendorid)+"</td>"
                   "<td>"+ phones +"</td>"
                   "<td align='right'>"+ bal +"</td>"
                   "</tr>";
            total_netopen += a_netopen;
        }
    }//end while
    QString cbal = erp.amountString(total_netopen)+" Dr";
    if(total_netopen<0)
    {
        cbal = erp.amountString(total_netopen * -1)+" Cr";
    }
    html +="<tr>"
               "<th colspan='4' align='right'>Total:</th>"
               "<th>"+cbal+"</th>"
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
                          "<th style='font-size:16px;'>Vendors Balance Report</th>"
                          "</tr>"
                          "<tr><th colspan='3' align='center'>For Date: "+erp.DDMMYYDateFromSQL(date)+"</th></tr>"
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
