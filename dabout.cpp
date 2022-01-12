#include "dabout.h"
#include <QtSql>
#include <QMessageBox>
//----------------------------------------------------------
dAbout::dAbout(QWidget *parent) : QDialog(parent)
{
    setupUi(this);
}
//----------------------------------------------------------
void dAbout::on_pbOk_clicked()
{
        reject();
}
//----------------------------------------------------------
