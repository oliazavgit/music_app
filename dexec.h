#ifndef DEXEC_H
#define DEXEC_H

#include "ui_dexec.h"
//----------------------------------------------------------
class dExec : public QDialog, private Ui::dExec
{
    Q_OBJECT
    QString sDBAlias;                                                           // Идентификатор доступа к БД
    QString sId;                                                                // Идентификатор записи
public:
    explicit dExec(QString ALIAS, QString ID = "", QWidget *parent = 0);
private slots:
    void on_pbOk_clicked();
    void on_pbCancel_clicked();
};
//----------------------------------------------------------
#endif // DEXEC_H
