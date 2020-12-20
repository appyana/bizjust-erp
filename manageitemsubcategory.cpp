#include "manageitemsubcategory.h"
#include "ui_manageitemsubcategory.h"

ManageItemSubCategory::ManageItemSubCategory(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ManageItemSubCategory)
{
    ui->setupUi(this);
    loadtable();
    ui->lbl_id->setVisible(false);
}

ManageItemSubCategory::~ManageItemSubCategory()
{
    delete ui;
}


void ManageItemSubCategory::loadtable()
{
    cb.loadItemCategoriesToComboBox(ui->catid,true);
    ui->detailtable->setColumnCount(3);
    ui->detailtable->setRowCount(0);
    QStringList titles;
    titles <<"id"<<"Category"<<"Sub Category";
    ui->detailtable->setHorizontalHeaderLabels(titles);
    ui->detailtable->hideColumn(ID);
    ui->detailtable->horizontalHeader()->setSectionResizeMode(CATEGORY, QHeaderView::Stretch);
    ui->detailtable->horizontalHeader()->setSectionResizeMode(NAME, QHeaderView::Stretch);
    QSqlQuery qry(" select * from pro_sub_cat order by id asc ");
    while(qry.next())
    {
        int row = ui->detailtable->rowCount();
        ui->detailtable->insertRow(row);
        ui->detailtable->setItem(row,ID, new QTableWidgetItem( qry.value("id").toString() ));
        ui->detailtable->setItem(row,CATEGORY, new QTableWidgetItem( erp.categoryName(qry.value("catid").toString() ) ));
        ui->detailtable->setItem(row,NAME, new QTableWidgetItem( qry.value("name").toString() ));
    }
}

void ManageItemSubCategory::on_save_btn_clicked()
{
    QString name = ui->name->text();
    QString catid = ui->catid->itemData(ui->catid->currentIndex()).toString();
    if(name=="")
    {
        QMessageBox::warning(this,"","Name is required");
        ui->name->setFocus();
        return;
    }
    if(catid=="0" || catid=="")
    {
        QMessageBox::warning(this,"","Category is required");
        ui->catid->setFocus();
        return;
    }
    QString sql;
    if(editid=="")
    {
        sql = " INSERT INTO pro_sub_cat "
              " (`name`,`catid`) "
              " VALUES "
              " ('"+name+"','"+catid+"') ";
    }
    else
    {
        sql = " UPDATE pro_sub_cat SET "
              " `name`     = '"+name+"',"
              " `catid`     = '"+catid+"'"
              " WHERE "
              " `id` = '"+editid+"'";
    }
    if(conn.execQuery(sql))
    {
        QMessageBox::information(this,"","Record saved successfully");
        loadtable();
        ui->name->clear();
        editid="";
        ui->lbl_id->clear();
        ui->lbl_id->setVisible(false);
    }
    else
    {
        QMessageBox::critical(this,"","Some problem in record insertion");
    }
}

void ManageItemSubCategory::on_edit_btn_clicked()
{
    if(ui->detailtable->currentRow()>=0)
    {
        int row = ui->detailtable->currentRow();
        QString id = ui->detailtable->item(row,ID)->text();
        QString name = ui->detailtable->item(row,NAME)->text();
        QSqlQuery qry;
        qry.prepare(" select * from pro_sub_cat WHERE id = '"+id+"' ");
        if(qry.exec())
        {
            ui->lbl_id->setVisible(true);
            editid = id;
            ui->lbl_id->setText("Sub Category: "+name);
            qry.next();
            int catid = qry.value("catid").toInt();
            ui->name->setText( qry.value("name").toString() );
            erp.setComboBoxSelected( ui->catid, catid );
        }
    }
    else
    {
        QMessageBox::warning(this,"","Select row for edit");
    }
}

void ManageItemSubCategory::on_clear_btn_clicked()
{
    ui->name->clear();
    ui->catid->setCurrentIndex(0);
    ui->lbl_id->clear();
    editid="";
    ui->lbl_id->setVisible(false);
}

void ManageItemSubCategory::on_delete_btn_clicked()
{
    if(ui->detailtable->currentRow()>=0)
    {

        QMessageBox::StandardButton reply;
          reply = QMessageBox::question(this, "Confirm", "Are you sure you want to delete this?",
                                        QMessageBox::Yes|QMessageBox::No);
          if (reply == QMessageBox::Yes) {
                int row = ui->detailtable->currentRow();
                QString id = ui->detailtable->item(row,ID)->text();
                if(conn.execQuery(" delete from pro_sub_cat WHERE id = '"+id+"' "))
                {
                    QMessageBox::information(this,"","Record deleted successfully");
                    loadtable();
                }
                else
                {
                    QMessageBox::critical(this,"","Some problem in record deletion");
                }
          }
    }
    else
    {
        QMessageBox::warning(this,"","Select row for delete");
    }
}
