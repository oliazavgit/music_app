#ifndef DGRAFIC_H
#define DGRAFIC_H

#include "ui_dgrafic.h"
#include <QtCharts>
#include <QtCore>
//----------------------------------------------------------
class dGrafic : public QDialog, private Ui::dGrafic
{
    Q_OBJECT
    QString sDBAlias;                                                           // Идентификатор доступа к БД
    dGrafic *cv;

public:
    explicit dGrafic(QString ALIAS,QWidget *parent = nullptr);
private slots:
    void on_pbOk_clicked();
};
//----------------------------------------------------------
#endif // DGRAFIC_H
