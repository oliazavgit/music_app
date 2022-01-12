#include "dexec.h"
#include <QtSql>
#include <QMessageBox>
//----------------------------------------------------------
dExec::dExec(QString ALIAS, QString ID, QWidget *parent) : QDialog(parent)      // выводим всё
{
    setupUi(this);
    sDBAlias = ALIAS;
    sId      = ID;
    if(!sId.isEmpty())
    {
        QSqlQuery q(QSqlDatabase::database(sDBAlias));
        QString sSql = "SELECT * FROM executor WHERE ID_executor=%1";
        q.exec(sSql.arg(sId));
        if(q.next())
        {
            leName->setText(q.value("name").toString());
            leSurname->setText(q.value("surname").toString());
            leNametwo->setText(q.value("twoname").toString());
        }
    }
}
//----------------------------------------------------------
void dExec::on_pbOk_clicked()                                                   // кнопка "Ок"
{
   if(leSurname->text().isEmpty())
    {
        QMessageBox::question(this, " ", tr("Не введена фамилия."), QMessageBox::Ok);
        leSurname->setFocus();
        return;
    }
    QSqlQuery q(QSqlDatabase::database(sDBAlias));
    if(sId.isEmpty())                                                           // добавляем исполнителя
    {
        QString sSql = "SELECT ID_executor FROM executor WHERE name='%1' AND surname='%2' AND twoname='%3'";
        q.exec(sSql.arg(leName->text()).arg(leSurname->text()).arg(leNametwo->text()));
        if(q.next())                                                            // если идентичное есть
        {
            QMessageBox::question(this, " ", tr("Такая запись уже есть."), QMessageBox::Ok);
            leName->clear();
            leSurname->clear();
            leNametwo->clear();
            leName->setFocus();
            return;
        }
        sSql = "INSERT INTO executor (name, surname, twoname) VALUES ('%1', '%2', '%3')";
        q.exec(sSql.arg(leName->text()).arg(leSurname->text()).arg(leNametwo->text()));
    }
    else                                                                        // изменяем исполнителя
    {
        QString sSql = "SELECT ID_executor FROM executor WHERE name='%1' AND surname='%2' AND twoname='%3' AND ID_executor<>%4"; //не мы сами
        q.exec(sSql.arg(leName->text()).arg(leSurname->text()).arg(leNametwo->text()).arg(sId));
        if(q.next())
        {
            QMessageBox::question(this, " ", tr("Такая запись уже есть."), QMessageBox::Ok); // если идентичное есть
            return;
        }
        sSql = "UPDATE executor SET name='%1', surname='%2', twoname='%3' WHERE ID_executor=%4";
        q.exec(sSql.arg(leName->text()).arg(leSurname->text()).arg(leNametwo->text()).arg(sId));
    }
    accept();
}
//----------------------------------------------------------
void dExec::on_pbCancel_clicked()
{
  reject();
}
//----------------------------------------------------------
