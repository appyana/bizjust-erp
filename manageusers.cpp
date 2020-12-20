#include "manageusers.h"
#include "ui_manageusers.h"

ManageUsers::ManageUsers(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ManageUsers)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);
    loadusers();
}

ManageUsers::~ManageUsers()
{
    delete ui;
}

void ManageUsers::loadusers()
{
    ui->user->clear();
    QSqlQuery qry(" select * from tblusers ");
    while(qry.next())
    {
        ui->user->addItem(qry.value("adminid").toString(),qry.value("id"));
    }
}

void ManageUsers::on_btn_add_clicked()
{
    editid="";
    ui->lbl_user->setVisible(false);
    ui->lbl_username->setVisible(false);
    ui->username->clear();
    ui->firstname->clear();
    ui->lastname->clear();
    ui->email->clear();
    ui->password->clear();
    ui->lbl_active->setVisible(true);
    ui->active->setVisible(true);
    ui->stackedWidget->setCurrentIndex(1);
    ui->username->setFocus();
}

void ManageUsers::on_btn_edit_clicked()
{
    QString id = ui->user->itemData(ui->user->currentIndex()).toString();
    editid=id;
    QSqlQuery qry(" select * from tblusers where id = '"+id+"' ");
    qry.next();
    ui->lbl_user->setVisible(true);
    ui->lbl_username->setVisible(true);
    ui->lbl_username->setText( qry.value("adminid").toString() );
    ui->username->setText( qry.value("adminid").toString() );
    ui->firstname->setText( qry.value("fname").toString() );
    ui->lastname->setText( qry.value("lname").toString() );
    ui->email->setText( qry.value("email").toString() );
    if(id.toInt()==1)
    {
        ui->lbl_active->setVisible(false);
        ui->active->setVisible(false);
    }
    else
    {
        ui->lbl_active->setVisible(true);
        ui->active->setVisible(true);
    }
    ui->active->setChecked( qry.value("isactive").toBool() );
    ui->stackedWidget->setCurrentIndex(1);
    ui->username->setFocus();
}


void ManageUsers::on_btn_back_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void ManageUsers::on_btn_save_clicked()
{
    QString adminid = ui->username->text();
    QString fname = ui->firstname->text();
    QString lname = ui->lastname->text();
    QString email = ui->email->text();
    QString pass = ui->password->text();
    QString password = QString(QCryptographicHash::hash((pass.toUtf8()),QCryptographicHash::Md5).toHex());
    QString active = "0";
    if(ui->active->isChecked()==true)
    {
        active = "1";
    }
    if(editid=="")
    {
        QSqlQuery qry(" select * from tblusers where adminid = '"+adminid+"' ");
        if(qry.size()>0)
        {
            QMessageBox::critical(this,"","username already exits");
            return;
        }
        if(adminid=="")
        {
            QMessageBox::critical(this,"","username is required");
            return;
        }
        if(pass=="")
        {
            QMessageBox::critical(this,"","password is required");
            return;
        }
        QString query = " insert into tblusers "
                        " ( `adminid`, `password`, `fname`, `lname`, `email`, `isactive` )"
                        " VALUES"
                        " ( '"+adminid+"','"+password+"','"+fname+"','"+lname+"','"+email+"','"+active+"' ) ";
        if(conn.execQuery(query))
        {
            QMessageBox::information(this,"","User "+adminid+" is inserted successfully ");
            ui->stackedWidget->setCurrentIndex(0);
            loadusers();
        }
        else
        {
            QMessageBox::critical(this,"","Some problem in record insertion");
        }
    }
    else
    {
        if(adminid=="")
        {
            QMessageBox::critical(this,"","username is required");
            return;
        }
        QString query = " update tblusers "
                        " set "
                        " `adminid`  = '"+adminid+"',"
                        " `fname`    = '"+fname+"',"
                        " `lname`    = '"+lname+"',"
                        " `email`    = '"+email+"',"
                        " `isactive` = '"+active+"' )"
                        " WHERE "
                        " id = '"+editid+"' ";
        if(pass!="")
        {
            query = " update tblusers "
                    " set "
                    " `adminid`  = '"+adminid+"',"
                    " `password` = '"+password+"',"
                    " `fname`    = '"+fname+"',"
                    " `lname`    = '"+lname+"',"
                    " `email`    = '"+email+"',"
                    " `isactive` = '"+active+"' )"
                    " WHERE "
                    " id = '"+editid+"' ";
        }
        if(conn.execQuery(query))
        {
            QMessageBox::information(this,"","User "+adminid+" is updated successfully ");
            ui->stackedWidget->setCurrentIndex(0);
            loadusers();
        }
        else
        {
            QMessageBox::critical(this,"","Some problem in record insertion");
        }
    }
}

void ManageUsers::on_btn_rights_clicked()
{
    ui->rightsTable->setColumnCount(3);
    QStringList titles;
    titles <<"ID"<<"Menu Title"<<"Rights";
    ui->rightsTable->setHorizontalHeaderLabels(titles);
    ui->rightsTable->hideColumn(ID);
    ui->rightsTable->setColumnWidth(ACTION,60);
    ui->rightsTable->horizontalHeader()->setSectionResizeMode(TITLE, QHeaderView::Stretch);
    QString id = ui->user->itemData(ui->user->currentIndex()).toString();
    editid=id;
    if(id.toInt()>1)
    {
        ui->module->addItem("Select",QVariant(0));
        QSqlQuery qry(" select * from tblmenue where type = 'main' and status='1' order by `sort` asc ");
        while(qry.next())
        {
            ui->module->addItem(qry.value("title").toString(),qry.value("id"));
        }
        ui->stackedWidget->setCurrentIndex(2);
    }

}

void ManageUsers::on_module_currentIndexChanged(int index)
{
    ui->rightsTable->setRowCount(0);
    QString parent_id = ui->module->itemData(index).toString();
    QSqlQuery qry_sub(" select * from tblmenue where type = 'sub' and status='1' and parent_id = '"+parent_id+"' order by `sort` asc ");
    while(qry_sub.next())
    {
        QSqlQuery qrychild(" select * from tblmenue where type = 'child' and status='1' and parent_id = '"+qry_sub.value("id").toString()+"' order by `sort` asc ");
        while(qrychild.next())
        {
            int row = ui->rightsTable->rowCount();
            ui->rightsTable->insertRow(row);

            //QWidget* pWidget = new QWidget();
            QCheckBox *chk = new QCheckBox();
            //chk->setContentsMargins(20,5,0,5);
            //chk->setGeometry(10,10,10,10);
            //chk->setObjectName("chk_"+qrychild.value("id").toString()  );
            //chk->setFocusPolicy(Qt::NoFocus);
//            QHBoxLayout* pLayout = new QHBoxLayout(pWidget);
//            pLayout->addWidget(chk);
//            pLayout->setAlignment(Qt::AlignCenter);
//            pLayout->setContentsMargins(0, 0, 0, 0);
//            pWidget->setLayout(pLayout);
            //pWidget->setFocusPolicy(Qt::NoFocus);

            //chk->setText("Active");
            //QTableWidgetItem *action = new QTableWidgetItem("Active");
            //action->setCheckState(Qt::Unchecked);
            ui->rightsTable->setItem(row,ID,new QTableWidgetItem( qrychild.value("id").toString() ));
            ui->rightsTable->setItem(row,TITLE,new QTableWidgetItem( qrychild.value("title").toString() ));
            ui->rightsTable->setCellWidget(row,ACTION, chk);
            ui->rightsTable->cellWidget(row,ACTION)->setStyleSheet("margin-left:20px;");
            connect(chk,SIGNAL(toggled(bool)), this, SLOT(chk_clicked(bool)) );
        }
    }
}

void ManageUsers::chk_clicked(bool arg1)
{
    if(arg1==true)
    {
        QString id = ui->rightsTable->item(ui->rightsTable->currentRow(),ID)->text();
        QSqlQuery qry(" select * from rights where userid = '"+editid+"' and menuid = '"+id+"' ");
        if(qry.size()==0)
        {
            conn.execQuery("insert into rights (userid, menuid) values ('"+editid+"','"+id+"')");
        }
        //QMessageBox::information(this,"","checked"+id);
    }
    else
    {
        QString id = ui->rightsTable->item(ui->rightsTable->currentRow(),ID)->text();
        conn.execQuery(" delete from rights where userid = '"+editid+"' and menuid = '"+id+"' ");
        //QMessageBox::information(this,"","unchecked");
    }
}

void ManageUsers::on_back_btn_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void ManageUsers::on_btn_delete_clicked()
{
    if(editid==1)
    {
        QMessageBox::critical(this,"","You can't delete admin user");
        return;
    }
    QMessageBox::StandardButton reply;
      reply = QMessageBox::question(this, "Confirm", "Are you sure you want to delete this?",
                                    QMessageBox::Yes|QMessageBox::No);
      if (reply == QMessageBox::Yes) {
            if(conn.execQuery(" delete from tblusers WHERE id = '"+editid+"' "))
            {
                conn.execQuery(" delete from rights WHERE userid = '"+editid+"' ");
                QMessageBox::information(this,"","User is deleted successfully.");
                ui->stackedWidget->setCurrentIndex(0);
                loadusers();
            }
            else
            {
                QMessageBox::critical(this,"","Some problem in record deletion");
            }
      }
}
