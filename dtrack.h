#ifndef DTRACK_H
#define DTRACK_H

#include "ui_dtrack.h"
//----------------------------------------------------------
class dTrack : public QDialog, private Ui::dTrack
{
    Q_OBJECT
    QString sDBAlias;                                                           //<Идентификатор доступа к БД
    QString sId;                                                                //<Идентификатор записи
    QString sIdExec;                                                            //<Идентификатор записи
    QString sIdStyle;                                                           //<Идентификатор записи
public:
      explicit dTrack(QString ALIAS, QString ID = "", QWidget *parent = 0);
private slots:
    void on_pbOk_clicked();
    void on_pbCancel_clicked();
    void on_pbPath_clicked();
};
//----------------------------------------------------------
#endif // DTRACK_H
