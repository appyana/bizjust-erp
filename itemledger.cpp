#include "itemledger.h"
#include "ui_itemledger.h"

ItemLedger::ItemLedger(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ItemLedger)
{
    ui->setupUi(this);
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(loadParams()) );
    timer->start(5);
}

ItemLedger::~ItemLedger()
{
    delete ui;
}


void ItemLedger::autocompleter(QString sql, QLineEdit *name_txt, QLineEdit *id_txt)
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

void ItemLedger::onItemHighlighted(const QModelIndex &index)
{
    QString code = index.data(Qt::UserRole).toString();
    QString sname = index.data(0).toString();
    sch.searchname = sname;
    sch.searchid = code;
    sch.id_txt->setText(code);
}
void ItemLedger::editingFinished()
{
    QString sname = sch.name_txt->text();
    QString sid = sch.id_txt->text();
    if(sname!=sch.searchname || sid != sch.searchid)
    {
        sch.name_txt->setText("");
        sch.id_txt->setText("");
    }
}

void ItemLedger::loadParams()
{
    timer->stop();
    ui->begindate->setDate( erp.reportBeginDate() );
    ui->enddate->setDate( QDate::currentDate() );
}

void ItemLedger::on_itemname_textEdited(const QString &arg1)
{
    autocompleter(sch.items(arg1),ui->itemname,ui->itemid);
}

void ItemLedger::on_btn_next_clicked()
{
    if(ui->itemid->text()=="")
    {
        QMessageBox::warning(this,"","Please select Item");
        ui->itemname->setFocus();
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
    pd->setWindowTitle("Item Ledger");
    pd->setWindowFlags(Qt::Window);
    pd->resize(900,600);
    pd->show();
}

void ItemLedger::printPreview(QPrinter *p)
{
    QString begindate = ui->begindate->date().toString("yyyy-MM-dd");
    QString enddate = ui->enddate->date().toString("yyyy-MM-dd");
    QString itemid = ui->itemid->text();

/******************************************report detail***********************************************/
    html = "";

    html += "<table width='100%' border='1' cellspacing='0' cellpadding='3'>"
            "<thead>"
            "<tr>"
            "<th colspan='8'>Item: "+erp.itemname(ui->itemid->text())+"</th>"
            "</tr>";
    int a_netopen = erp.openingStockCalculation(itemid, "", begindate,false).toInt();
    html +="<tr bgcolor='#f2f2f2'>"
               "<th width='10%'>Date</th>"
               "<th width='5%'>Trans #</th>"
               "<th width='20%'>Customer/Vendor</th>"
               "<th width='25%'>Description</th>"
               "<th width='10%'>Location</th>"
               "<th width='10%'>Qty In</th>"
               "<th width='10%'>Qty Out</th>"
               "<th width='10%'>Net Qty</th>"
           "</tr></thead>"
           "<tr>"
           "<th colspan='7' align='right'>Begining Stock:</th>"
           "<th>"+erp.intString(a_netopen)+"</th>"
           "</tr>";


    QString query_s= "select *, itd.ITransID itid,itd.ITransType ittype from tblitemtrans it, tblitemtransdet itd "
                     " where it.itransid= itd.itransid and it.itranstype=itd.itranstype and itd.itemid='"+itemid+"' "
                     " and it.ITDate between '"+begindate+"' and '"+enddate+"' order by it.ITDate, it.itransid asc ";
    int bal=0;
    int net_qtyin=0;
    int net_qtyout=0;
    QSqlQuery info_s(query_s);
    while(info_s.next())
    {
        QString flag="on";
        QString ITDate  = erp.DDMMYYDateFromSQL(info_s.value("ITDate").toString() );
        QString itid    = info_s.value("itid").toString();
        QString ittype  = info_s.value("ittype").toString();
        QString transNo = ittype+"-"+itid;
        QString ItemID = info_s.value("ItemID").toString();
        int Qty = info_s.value("Qty").toInt();
        QString cname   = "";
        if(ittype == "SI" || ittype == "si")
        {
            QSqlQuery cus("SELECT customerid FROM tblinvoice WHERE invoicenumber = '"+transNo+"' ");
            cus.next();
            cname = erp.customerName(cus.value("customerid").toString());
        }
        if(ittype == "SR" || ittype == "sr")
        {
            cname = erp.customerName(info_s.value("SenderRecieverID").toString());
        }
        if(ittype == "PI" || ittype == "pi")
        {
            QSqlQuery ven("SELECT vendorid FROM tblinvporcv WHERE voucherno = '"+transNo+"' ");
            ven.next();
            cname = erp.vendorname( ven.value("vendorid").toString() );
        }
        if(ittype == "PR" || ittype == "pr"){
            QSqlQuery ven("SELECT vendorid FROM tblpurchasereturn WHERE prid = '"+transNo+"'");
            ven.next();
            cname = erp.vendorname( ven.value("vendorid").toString() );
        }
        QString packingUnit = info_s.value("packingUnit").toString();
        QString CalType = info_s.value("CalType").toString();
        int qty_in=0;
        int qty_out=0;
        if(CalType=="+")
        {
            qty_in = erp.stockInSmallPacket(ItemID, Qty, packingUnit);
            a_netopen += qty_in;
            net_qtyin += qty_in;
            qty_out = 0;
        }
        else if(CalType=="-")
        {
            qty_out = erp.stockInSmallPacket(ItemID, Qty, packingUnit);
            a_netopen -= qty_out;
            net_qtyout += qty_out;
            qty_in=0;
        }
        else
        {
            qty_in=0;
            qty_out=0;
        }

        html +="<tr>"
               "<td align='center'>"+ITDate+"</td>"
               "<td align='center'>"+transNo+"</td>"
               "<td>"+ cname +"</td>"
               "<td>"+ info_s.value("DetailedDescription").toString() +"</td>"
               "<td>"+ erp.locationName( info_s.value("locationid").toString() ) +"</td>"
               "<td align='center'>"+ erp.showStockInPacking(qty_in, ItemID) +"</td>"
               "<td align='center'>"+ erp.showStockInPacking(qty_out, ItemID) +"</td>"
               "<td align='center'>"+ erp.showStockInPacking(a_netopen, ItemID) +"</td>"
               "</tr>";
    }//end while
    html += "<tr>"
            "<th colspan='5' align='right'>Ending Stock:</th>"
            "<th>"+erp.showStockInPacking(net_qtyin,itemid)+"</th>"
            "<th>"+erp.showStockInPacking(net_qtyout,itemid)+"</th>"
            "<th>"+erp.showStockInPacking(a_netopen,itemid)+"</th>"
            "</tr>";
    html += "</table>";

/******************************************* Header****************************************************/

    QString header_html = "<table width='100%'>"
                          "<tr>"
                          "<td rowspan='2' width='20%' valign='bottom'>Print Date: "+QDate::currentDate().toString("dd/MM/yyyy")+"</td>"
                          "<td width='60%' align='center' style='font-size:22px;text-transform:uppercase;'><b>"+erp.companyname()+"</b></td>"
                          "<td rowspan='2' width='20%' align='right' valign='bottom'></td>"
                          "</tr>"
                          "<tr>"
                          "<th style='font-size:16px;'>Item Ledger</th>"
                          "</tr>"
                          "<tr><th colspan='3' align='center'>From Date: "+erp.DDMMYYDateFromSQL(begindate)+" To Date: "+erp.DDMMYYDateFromSQL(enddate)+"</th></tr>"
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
