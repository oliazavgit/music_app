#include "dtrack.h"
#include <QtSql>
#include <QMessageBox>
#include <QFileDialog>
//----------------------------------------------------------
dTrack::dTrack(QString ALIAS, QString ID, QWidget *parent) : QDialog(parent)
{
    setupUi(this);
    sDBAlias = ALIAS;
    sId      = ID;
    sIdExec  = "";
    sIdStyle = "";
    QSqlQuery q(QSqlDatabase::database(sDBAlias));

    q.exec("SELECT * FROM executor");                                           //заполняем комбобокмы всеми значениями
    cbExec->clear();
    while(q.next())
    {
        cbExec->addItem(QString("%1 %2 %3")
                .arg(q.value("surname").toString())
                .arg(q.value("name").toString())
                .arg(q.value("twoname").toString()), q.value("ID_executor").toString());
    }

    q.exec("SELECT * FROM styles");                                             //заполняем комбобокмы всеми значениями
    cbStyle->clear();
    while(q.next())
    {
        cbStyle->addItem((q.value("title").toString()), q.value("ID_style").toString());
    }

    if(!sId.isEmpty())
    {
        QString sSql = "SELECT * FROM track WHERE ID_track=%1";
        q.exec(sSql.arg(sId));
        if(q.next())
        {
            leName->setText(q.value("name").toString());
            sIdExec  = q.value("ID_executor").toString();
            sIdStyle = q.value("ID_style").toString();
            lePath->setText(q.value("path").toString());

                                                                                //выбирает выбранного исполнителя и запихивает в cb
            for(int i=0; i<cbExec->count(); i++)
            {
                if(cbExec->itemData(i).toString() == sIdExec)
                {
                    cbExec->setCurrentIndex(i);
                    break;
                }
            }
                                                                                //выбирает выбранный стиль и запихивает в cb
            for(int i=0; i<cbStyle->count(); i++)
            {
                if(cbStyle->itemData(i).toString() == sIdStyle)
                {
                    cbStyle->setCurrentIndex(i);
                    break;
                }
            }

        }
    }
    else
    {
        cbExec->setCurrentIndex(-1);                                                //пустое значение
        cbStyle->setCurrentIndex(-1);                                               //пустое значение
    }
}
//----------------------------------------------------------                    // Ok
void dTrack::on_pbOk_clicked()
{
   if(leName->text().isEmpty())                                                 //нет имени
    {
        QMessageBox::question(this, " ", tr("Не введена название."), QMessageBox::Ok);
        leName->setFocus();
        return;
    }
    if(cbExec->currentIndex() == -1)                                            //нет исполнителя
    {
        QMessageBox::question(this, " ", tr("Не выбран исполнитель."), QMessageBox::Ok);
        cbExec->setFocus();
        return;
    }
    if(cbStyle->currentIndex() == -1)                                           //нет стиля
    {
        QMessageBox::question(this, " ", tr("Не выбран стиль."), QMessageBox::Ok);
        cbStyle->setFocus();
        return;
    }
   if(lePath->text().isEmpty())                                                 //нет имени
    {
        QMessageBox::question(this, " ", tr("Не введен путь к треку."), QMessageBox::Ok);
        lePath->setFocus();
        return;
    }

    QSqlQuery q(QSqlDatabase::database(sDBAlias));
    if(sId.isEmpty())                                                           //добавление
    {   QString sSql = "SELECT ID_track FROM track WHERE name='%1' "
                       "AND ID_executor=%2 AND ID_style=%3";
        q.exec(sSql.arg(leName->text())
            .arg(cbExec->currentData().toString())
            .arg(cbStyle->currentData().toString()));
        if(q.next())                                                            //если на существующую
        {
            QMessageBox::question(this, " ", tr("Такая запись уже есть."), QMessageBox::Ok);
            leName->clear();
            leName->setFocus();
            return;
        }
        sSql = "INSERT INTO track (name, ID_executor, ID_style, path) "
               "VALUES ('%1', %2, %3, '%4')";
        q.exec(sSql.arg(leName->text())
            .arg(cbExec->currentData().toString())
            .arg(cbStyle->currentData().toString())
            .arg(lePath->text()));
    }
    else
    {   QString sSql = "SELECT ID_track FROM track WHERE name='%1' AND "        //изменение
                       "ID_executor=%2 AND ID_style=%3 AND ID_track<>%4";
        q.exec(sSql.arg(leName->text())
            .arg(cbExec->currentData().toString())
            .arg(cbStyle->currentData().toString())
            .arg(sId));
        if(q.next())                                                            //изменение на уже существующее
        {
            QMessageBox::question(this, " ", tr("Такая запись уже есть."), QMessageBox::Ok);
            return;
        }
        sSql = "UPDATE track SET name='%1', ID_executor=%2, "
               "ID_style=%3, path='%4' WHERE ID_track=%5";
        q.exec(sSql.arg(leName->text())
            .arg(cbExec->currentData().toString())
            .arg(cbStyle->currentData().toString())
            .arg(lePath->text())
            .arg(sId));
//        qDebug()<<q.lastQuery();
//        qDebug()<<q.lastError();
    }
    accept();
}
//----------------------------------------------------------                    // Отмена
void dTrack::on_pbCancel_clicked()
{
   reject();
}
//----------------------------------------------------------                    //запихиваем возможность выбора пути через проводник
void dTrack::on_pbPath_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
      tr("Путь к треку"), lePath->text(), tr("MP3 Files (*.mp3)"));
    if(!fileName.isEmpty())
        lePath->setText(fileName);
}
//----------------------------------------------------------
