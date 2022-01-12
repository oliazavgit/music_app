#include "dreg.h"
#include <QtSql>
#include <QMessageBox>
//----------------------------------------------------------
dReg::dReg(QString ALIAS, QWidget *parent) : QDialog(parent)
{
    setupUi(this);
    sDBAlias = ALIAS;
}
//----------------------------------------------------------
void dReg::on_pbOk_clicked()
{   if(leLogin->text().isEmpty())                                               // не введено имя
    {
        QMessageBox::question(this, " ", tr("Не введено имя пользователя."), QMessageBox::Ok);
        leLogin->setFocus();
        return;
    }
    if(lePass_1->text().isEmpty())                                              // не введен пароль
    {
        QMessageBox::question(this, " ", tr("Пароль не может быть пустым."), QMessageBox::Ok);
        lePass_1->clear();
        lePass_2->clear();
        lePass_1->setFocus();
        return;
    }
    if(lePass_1->text() != lePass_2->text())                                    // второй =! первому
    {
        QMessageBox::question(this, " ", tr("Введенные пароли не совпадают."), QMessageBox::Ok);
        lePass_1->clear();
        lePass_2->clear();
        lePass_1->setFocus();
        return;
    }
    QSqlQuery q(QSqlDatabase::database(sDBAlias));
    QString sSql = "SELECT ID_user FROM users WHERE login='%1'";
    q.exec(sSql.arg(leLogin->text()));
    if(q.next())                                                                // если не пустое
    {
        QMessageBox::question(this, " ", tr("Пользователь с таким именем уже есть.\nУкажите другое имя."), QMessageBox::Ok);
        leLogin->clear();
        lePass_1->clear();
        lePass_2->clear();
        leLogin->setFocus();
        return;
    }
    sSql = "INSERT INTO users (login, password) VALUES ('%1', '%2')";           // добавляем пользователя
    q.exec(sSql.arg(leLogin->text()).arg(lePass_1->text()));
    accept();
}
//----------------------------------------------------------
void dReg::on_pbCancel_clicked()
{   reject();
}
//----------------------------------------------------------
