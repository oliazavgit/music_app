#ifndef DABOUT_H
#define DABOUT_H
//----------------------------------------------------------
#include "ui_dabout.h"
class dAbout : public QDialog, private Ui::dAbout
{
    Q_OBJECT

public:
    explicit dAbout(QWidget *parent = 0);
private slots:
    void on_pbOk_clicked();
};
//----------------------------------------------------------
#endif // DABOUT_H
