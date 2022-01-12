#ifndef DREG_H
#define DREG_H

#include "ui_dreg.h"
//----------------------------------------------------------
class dReg : public QDialog, private Ui::dReg
{
    Q_OBJECT
    QString sDBAlias;                                                           //<Идентификатор доступа к БД
public:
    explicit dReg(QString ALIAS, QWidget *parent = 0);
private slots:
    void on_pbOk_clicked();
    void on_pbCancel_clicked();
};
//----------------------------------------------------------
#endif // DREG_H
