#include "dstyle.h"
#include <QtSql>
#include <QMessageBox>
//----------------------------------------------------------
dStyle::dStyle(QString ALIAS, QString ID, QWidget *parent) : QDialog(parent)     // выводим всё
{
    setupUi(this);
    sDBAlias = ALIAS;
    sId      = ID;
    if(!sId.isEmpty())
    {
        QSqlQuery q(QSqlDatabase::database(sDBAlias));
        QString sSql = "SELECT * FROM styles WHERE ID_style=%1";
        q.exec(sSql.arg(sId));
        if(q.next())
        {
            leName->setText(q.value("title").toString());
            leAbout->setText(q.value("about").toString());
        }
    }
}
//----------------------------------------------------------
void dStyle::on_pbOk_clicked()                                                  // кнопка "Ок"
{
    if(leName->text().isEmpty())
    {
        QMessageBox::question(this, " ", tr("Не введено наименование."), QMessageBox::Ok);
        leName->setFocus();
        return;
    }
    QSqlQuery q(QSqlDatabase::database(sDBAlias));
    if(sId.isEmpty())                                                           // добавляем стиль
    {
        QString sSql = "SELECT ID_style FROM styles WHERE title='%1'";
        q.exec(sSql.arg(leName->text()));
        if(q.next())                                                            // если идентичное есть
        {
            QMessageBox::question(this, " ", tr("Такая запись уже есть."), QMessageBox::Ok);
            leName->clear();
            leAbout->clear();
            leName->setFocus();

            return;
        }
        sSql = "INSERT INTO styles (title, about) VALUES ('%1', '%2')";
        q.exec(sSql.arg(leName->text()).arg(leAbout->text()));
    }
    else
    {   QString sSql = "SELECT ID_style FROM styles WHERE title='%1' AND ID_style<>%2"; // изменяем стиль
        q.exec(sSql.arg(leName->text()).arg(sId));
        if(q.next())
        {
            QMessageBox::question(this, " ", tr("Такая запись уже есть."), QMessageBox::Ok);
            return;
        }
        sSql = "UPDATE styles SET title='%1', about='%2' WHERE ID_style=%3";
        q.exec(sSql.arg(leName->text()).arg(leAbout->text()).arg(sId));
    }
    accept();
}
//----------------------------------------------------------
void dStyle::on_pbCancel_clicked()
{
    reject();
}
//----------------------------------------------------------
