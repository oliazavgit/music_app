#ifndef MWMAIN_H
#define MWMAIN_H

#include "ui_mwmain.h"
#include <QtSql>
#include <QMediaPlayer>
//----------------------------------------------------------
class mwMain : public QWidget, private Ui::mwMain
{
    Q_OBJECT
    QString sDBHost;                                                            //IP адрес компьютера с БД
    QString sDBName;                                                            //Имя БД
    QString sDBUser;                                                            //Имя пользователя БД
    QString sDBPass;                                                            //Пароль пользователя БД
    int iDBPort;                                                                //Порт доступа к БД
    QString sDBAlias;                                                           //Идентификатор доступа к БД
    QString sIdUser;
    QSqlDatabase dbMuz;
    QMediaPlayer *player;
    int iTimerId;

    void setExec(void);
    void setStyle(void);
    void setTrack(void);
    void setTrackPlay(void);
    void setTrackFav(void);
    void my_update(void);
    void my_connectDb(void);
    void timerEvent(QTimerEvent *event);
public:
    explicit mwMain(QWidget *parent = nullptr);
    ~mwMain();
private slots:
    void my_viewPos(qint64 pos);
    void my_viewSize(qint64 dur);
    void on_pbAbout_clicked();
    void on_pbClose_clicked();
    void on_pbLogin_clicked();
    void on_pbReg_clicked();
    void on_pbAddExec_clicked();
    void on_pbEgitExec_clicked();
    void on_pbDelExec_clicked();
    void on_pbAddStyle_clicked();
    void on_pbEgitStyle_clicked();
    void on_pbDelStyle_clicked();
    void on_pbAddTrack_clicked();
    void on_pbEgitTrack_clicked();
    void on_pbDelTrack_clicked();
    void on_pbTrFind_clicked();
    void on_pbTrClear_clicked();
    void on_pbAddFav_clicked();
    void on_pbPlay_clicked();
    void on_pbDelFav_clicked();
    void on_twTrack_itemDoubleClicked(QTreeWidgetItem *item, int column);
    void on_twExec_itemDoubleClicked(QTreeWidgetItem *item, int column);
    void on_twStyle_itemDoubleClicked(QTreeWidgetItem *item, int column);
    void on_twTrackFav_itemDoubleClicked(QTreeWidgetItem *item, int column);
    void on_pbUser_clicked();
    void on_twExec_itemSelectionChanged();
    void on_twStyle_itemSelectionChanged();
    void on_twTrack_itemSelectionChanged();
    void on_pbGrafic_clicked();
    void on_twTrackPlay_itemDoubleClicked(QTreeWidgetItem *item, int column);
    void on_lePass_returnPressed();
    void on_hsTrack_sliderReleased();
};
//----------------------------------------------------------
#endif // MWMAIN_H
