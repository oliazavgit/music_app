#ifndef DSTYLE_H
#define DSTYLE_H

#include "ui_dstyle.h"
//----------------------------------------------------------

class dStyle : public QDialog, private Ui::dStyle
{
    Q_OBJECT
    QString sDBAlias;                                                           // Идентификатор доступа к БД
    QString sId;                                                                // Идентификатор записи
public:
    explicit dStyle(QString ALIAS, QString ID = "", QWidget *parent = 0);
private slots:
    void on_pbOk_clicked();
    void on_pbCancel_clicked();
};

#endif // DSTYLE_H
