#include "stockreport.h"
#include "ui_stockreport.h"

StockReport::StockReport(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StockReport)
{
    ui->setupUi(this);
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(loadParams()) );
    timer->start(5);
}

StockReport::~StockReport()
{
    delete ui;
}


void StockReport::autocompleter(QString sql, QLineEdit *name_txt, QLineEdit *id_txt)
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

void StockReport::onItemHighlighted(const QModelIndex &index)
{
    QString code = index.data(Qt::UserRole).toString();
    QString sname = index.data(0).toString();
    sch.searchname = sname;
    sch.searchid = code;
    sch.id_txt->setText(code);
}
void StockReport::editingFinished()
{
    QString sname = sch.name_txt->text();
    QString sid = sch.id_txt->text();
    if(sname!=sch.searchname || sid != sch.searchid)
    {
        sch.name_txt->setText("");
        sch.id_txt->setText("");
    }
}

void StockReport::loadParams()
{
    timer->stop();
    ui->date->setDate( QDate::currentDate() );
    cb.loadItemCategoriesToComboBox(ui->category);
    cb.loadItemSubCategoriesToComboBox(ui->subcategory);
    cb.loadItemTypesToComboBox(ui->itemtype);
    cb.loadItemBrandToComboBox(ui->brand);
}

void StockReport::on_vendorname_textEdited(const QString &arg1)
{
    autocompleter(sch.vendors(arg1),ui->vendorname,ui->vendorid);
}

void StockReport::on_btn_next_clicked()
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
    pd->setWindowTitle("Stock Report");
    pd->setWindowFlags(Qt::Window);
    pd->resize(900,600);
    pd->show();
}

void StockReport::printPreview(QPrinter *p)
{
    QString date = ui->date->date().toString("yyyy-MM-dd");
    QString vendorid = ui->vendorid->text();
    QString itemtype = ui->itemtype->itemData(ui->itemtype->currentIndex()).toString();
    QString category = ui->category->itemData(ui->category->currentIndex()).toString();
    QString subcategory = ui->subcategory->itemData(ui->subcategory->currentIndex()).toString();
    QString brand = ui->brand->itemData(ui->brand->currentIndex()).toString();
    bool zeroStock = ui->showzero->isChecked();
    bool negitiveStock = ui->shownegative->isChecked();

/******************************************report detail***********************************************/
    html = "";

    html += "<table width='100%' border='1' cellspacing='0' cellpadding='3'>"
            "<thead>";

    html +="<tr bgcolor='#f2f2f2'>"
               "<th width='15%'>Item Type</th>"
               "<th width='15%'>Category</th>"
               "<th width='10%'>Item Code</th>"
               "<th width='30%'>Item Name</th>"
               "<th width='10%'>Qty (Master Packing)</th>"
               "<th width='10%'>Qty (Large Packing)</th>"
               "<th width='10%'>Qty (Small Packing)</th>"
           "</tr></thead>";


    QString query = "SELECT itemid, itemcode, description, weight_inbasic_unit, lbsperpriceunit, priceunit, "
                    " categoryid, price1, itemtype, priceunit, NAME, masterpackingunitid, packpermasterunitid, "
                    " pcsperpackunitid FROM tblitems, tblitemcategory WHERE "
                    " tblitems.categoryid = tblitemcategory.id AND dl ='0' ";
    if(itemtype.toInt()>0)
    {
        query += " and itemtype='"+itemtype+"' ";
    }
    if(category.toInt()>0)
    {
        query += " and categoryid='"+category+"' ";
    }
    if(subcategory.toInt()>0)
    {
        query += " and subcatid='"+subcategory+"' ";
    }
    if(brand.toInt()>0)
    {
        query += " and brandid='"+brand+"' ";
    }
    if(vendorid!="")
    {
        query += " and vendorid='"+vendorid+"' ";
    }
    query += " order by description asc ";
    qDebug()<<query<<endl;

    QSqlQuery info(query);
    while(info.next())
    {
        QString itemid = info.value("itemid").toString();
        if(
                (zeroStock==true && negitiveStock==true)
                ||
                (zeroStock==true && negitiveStock==false && erp.stockCalculationSmallPacket(itemid,"",date)>=0)
                ||
                (zeroStock==false && negitiveStock==true && erp.stockCalculationSmallPacket(itemid,"",date)!=0)
                ||
                (zeroStock==false && negitiveStock==false && erp.stockCalculationSmallPacket(itemid,"",date)>0)
          )
        {
            QString stokInHand = erp.stockCalculation(itemid,"",date);
            QString largePack = stokInHand.split(QRegExp("  ")).value(0);
            QString mediumPack="";
            QString smallPack="";
            if(info.value("packpermasterunitid").toInt()!=0)
            {
                mediumPack = stokInHand.split(QRegExp("  ")).value(1);
            }
            if(info.value("pcsperpackunitid").toInt()!=0)
            {
                if(info.value("packpermasterunitid").toInt()==0)
                {
                    smallPack = stokInHand.split(QRegExp("  ")).value(1);
                }
                else
                {
                    smallPack = stokInHand.split(QRegExp("  ")).value(2);
                }
            }
            html +="<tr>"
                   "<td>"+erp.showItemType(info.value("itemtype").toString())+"</td>"
                   "<td>"+erp.categoryName(info.value("categoryid").toString())+"</td>"
                   "<td>"+ info.value("itemcode").toString() +"</td>"
                   "<td>"+ info.value("description").toString() +"</td>"
                   "<td>"+largePack+"</td>"
                   "<td>"+mediumPack+"</td>"
                   "<td>"+smallPack+"</td>"
                   "</tr>";
        }//end if
    }//end while

    html +="</table>";

/******************************************* Header****************************************************/

    QString header_html = "<table width='100%'>"
                          "<tr>"
                          "<td rowspan='2' width='20%' valign='bottom'>Print Date: "+QDate::currentDate().toString("dd/MM/yyyy")+"</td>"
                          "<td width='60%' align='center' style='font-size:22px;text-transform:uppercase;'><b>"+erp.companyname()+"</b></td>"
                          "<td rowspan='2' width='20%' align='right' valign='bottom'></td>"
                          "</tr>"
                          "<tr>"
                          "<th style='font-size:16px;'>Stock Report</th>"
                          "</tr>"
                          "<tr><td colspan='3' align='center'>Till Date: "+erp.DDMMYYDateFromSQL(date)+"</td></tr>"
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
