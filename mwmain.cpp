#include "mwmain.h"
#include <QDebug>
#include <QMessageBox>
#include "dreg.h"
#include "dexec.h"
#include "dstyle.h"
#include "dtrack.h"
#include "dabout.h"
#include "dgrafic.h"
//----------------------------------------------------------
mwMain::mwMain(QWidget *parent) :  QWidget(parent)
{
    setupUi(this);
    player   = nullptr;
    sDBAlias = "MUZDB";
    sIdUser  = "";

    QString sIniPath = QString("%1/pkserver.ini").arg(QApplication::applicationDirPath()); //текущее приложение и там ini
    QSettings setting(sIniPath, QSettings::IniFormat, nullptr);                 //родительского нет, формат ini,путь ранее указали
    sDBHost = setting.value("DBHost").toString();
    iDBPort = setting.value("DBPort").toInt();
    sDBName = setting.value("DBName").toString();
    sDBUser = setting.value("DBUser").toString();
    sDBPass = setting.value("DBPass").toString();

    dbMuz = QSqlDatabase::addDatabase("QPSQL", sDBAlias);
    dbMuz.setHostName(sDBHost);
    dbMuz.setPort(iDBPort);
    dbMuz.setDatabaseName(sDBName);
    dbMuz.setUserName(sDBUser);
    dbMuz.setPassword(sDBPass);

    twExec->hideColumn(0);                                                      //скрываем служебные столбцы списка
    twStyle->hideColumn(0);
    twTrack->hideColumn(0);
    twTrackPlay->hideColumn(0);
    twTrackFav->hideColumn(0);
    twTrackPlay->hideColumn(4);
    twTrackFav->hideColumn(4);
                    //----------------------------------------------------------
    pbPlay->setIcon(QApplication::style()->standardIcon(QStyle::SP_MediaPlay)); //стандартные иконки
    pbStop->setIcon(QApplication::style()->standardIcon(QStyle::SP_MediaStop));

    pbAbout->setIcon(QApplication::style()->standardIcon(QStyle::SP_FileLinkIcon));
    pbUser->setIcon(QApplication::style()->standardIcon(QStyle::SP_BrowserReload));
    pbClose->setIcon(QApplication::style()->standardIcon(QStyle::SP_DialogCancelButton));


    pbLogin->setIcon(QApplication::style()->standardIcon(QStyle::SP_DialogOkButton));
    pbReg->setIcon(QApplication::style()->standardIcon(QStyle::SP_DialogYesButton));

                    //----------------------------------------------------------
    player = new QMediaPlayer(this);                                            //создаем плеер
    if(player)
    {
        player->setVolume(100);
        connect(pbStop, SIGNAL(clicked(bool)), player, SLOT(stop()));           //событие стоп
        connect(dVollume, SIGNAL(valueChanged(int)), player, SLOT(setVolume(int)));//событие громкости
        connect(player, SIGNAL(positionChanged(qint64)), this, SLOT(my_viewPos(qint64)));//событие изменение позиции
        connect(player, SIGNAL(durationChanged(qint64)), this, SLOT(my_viewSize(qint64)));//событие размера
    }
                    //----------------------------------------------------------
    swMain->setCurrentWidget(pLoad);
    iTimerId = startTimer(100);
}
//----------------------------------------------------------
mwMain::~mwMain()                                                               //убили плеер
{
    if(player)
    {
        delete player;
        player = nullptr;
    }
}
//----------------------------------------------------------                    //соединение с БД
void mwMain::my_connectDb()
{
    bool bOpen = dbMuz.open();
    if(bOpen)                                                                   //если открылось
    {
        swMain->setCurrentWidget(pLogin);
        leLogin->setFocus();
    }
    else
        swMain->setCurrentWidget(pMsg);
}
//----------------------------------------------------------                    //таймер
void mwMain::timerEvent(QTimerEvent *)
{
    killTimer(iTimerId);
    my_connectDb();
}
//----------------------------------------------------------
//заполнение у пользователя
//----------------------------------------------------------                    //комбобоксы
void mwMain::my_update()                                                        //заполнение у пользователя
{
    QSqlQuery q(QSqlDatabase::database(sDBAlias));

    q.exec("SELECT * FROM executor");                                           //заполнение выбора исполнителя у пользователя
    cbTrExec->clear();
    while(q.next())
    {
        cbTrExec->addItem(QString("%1 %2 %3")
                          .arg(q.value("surname").toString())
                          .arg(q.value("name").toString())
                          .arg(q.value("twoname").toString()), q.value("ID_executor").toString());
    }
    cbTrExec->setCurrentIndex(-1);

    q.exec("SELECT * FROM styles");                                             //заполнение выбора стиля у пользователя
    cbTrStyle->clear();
    while(q.next())
    {
        cbTrStyle->addItem((q.value("title").toString()), q.value("ID_style").toString());
    }
    cbTrStyle->setCurrentIndex(-1);
}
//----------------------------------------------------------
void mwMain::setTrackFav()                                                      //понравившихся треков пользователя
{
    twTrackFav->clear();
    QSqlQuery q(QSqlDatabase::database(sDBAlias));
    QString stSql = "SELECT tr.ID_track, tr.name, tr.path, "
                    "ex.name as ex_name, ex.surname, ex.twoname, "
                    "st.title, "
                    "fv.ID_favorites "
                    "FROM track as tr, executor as ex, styles as st, favorites as fv  "
                    "WHERE ex.ID_executor=tr.ID_executor AND st.ID_style=tr.ID_style "
                    "AND tr.ID_track=fv.ID_track AND ID_user=%1";
    q.exec(stSql.arg(sIdUser));
    while(q.next())
    {
        QTreeWidgetItem *trwi = new QTreeWidgetItem(twTrackFav);
        if(trwi)
        {
            trwi->setText(0, q.value("ID_favorites").toString());
            trwi->setText(1, q.value("name").toString());
            trwi->setText(2, QString("%1\n%2 %3").arg(q.value("surname").toString()).arg(q.value("ex_name").toString()).arg(q.value("twoname").toString()));
            trwi->setText(3, q.value("title").toString());
            trwi->setText(4, q.value("path").toString());
        }
    }
}
//----------------------------------------------------------
void mwMain::setTrackPlay()                                                     //заполнение треками + организация поиска
{
    twTrackPlay->clear();
    QSqlQuery q(QSqlDatabase::database(sDBAlias));
    QString sSqlAdd = "";
    if(!leTrName->text().isEmpty())
        sSqlAdd += QString(" AND lower(tr.name) LIKE '%%1%'").arg(leTrName->text().toLower());
    if(cbTrExec->currentIndex() != -1)
        sSqlAdd += QString(" AND ex.ID_executor=%1").arg(cbTrExec->currentData().toString());
    if(cbTrStyle->currentIndex() != -1)
        sSqlAdd += QString(" AND st.ID_style=%1").arg(cbTrStyle->currentData().toString());
    QString sSql = "SELECT tr.ID_track, tr.name, tr.path, "
                   "ex.name as ex_name, ex.surname, ex.twoname, "
                   "st.title "
                   "FROM track as tr, executor as ex, styles as st "
                   "WHERE ex.ID_executor=tr.ID_executor AND st.ID_style=tr.ID_style%1";
    q.exec(sSql.arg(sSqlAdd));
    while(q.next())
    {
        QTreeWidgetItem *trwi = new QTreeWidgetItem(twTrackPlay);
        if(trwi)
        {
            trwi->setText(0, q.value("ID_track").toString());
            trwi->setText(1, q.value("name").toString());
            trwi->setText(2, QString("%1\n%2 %3").arg(q.value("surname").toString()).arg(q.value("ex_name").toString()).arg(q.value("twoname").toString()));
            trwi->setText(3, q.value("title").toString());
            trwi->setText(4, q.value("path").toString());
        }
    }
}
//----------------------------------------------------------
void mwMain::on_pbTrFind_clicked()                                              //кнопка поиска
{
    setTrackPlay();
}
//----------------------------------------------------------
void mwMain::on_pbTrClear_clicked()                                             //кнопка отчистки поиска
{
    leTrName->clear();
    cbTrExec->setCurrentIndex(-1);
    cbTrStyle->setCurrentIndex(-1);
    setTrackPlay();
}
//----------------------------------------------------------
void mwMain::on_pbAddFav_clicked()                                              // добавить в понравившиеся
{
    QSqlQuery q(QSqlDatabase::database(sDBAlias));

    QString strSql = "SELECT ID_track "
                     "FROM favorites "
                     "WHERE  ID_user=%1 AND ID_track=%2 ";
    q.exec(strSql.arg(sIdUser).arg(twTrackPlay->selectedItems().at(0)->text(0)));
    if(q.next())                                                                //если на существующую
        QMessageBox::question(this, " ", tr("Такая запись уже есть."), QMessageBox::Ok);
    else
    {
        QString sSql = "INSERT INTO favorites (ID_user, ID_track) VALUES (%1, %2)";
        q.exec(sSql.arg(sIdUser).arg(twTrackPlay->selectedItems().at(0)->text(0)));
    }
    setTrackFav();
}
//----------------------------------------------------------                    //удалить из понравившегося
void mwMain::on_pbDelFav_clicked()
{
    if(twTrackFav->selectedItems().count())
    {
        if(QMessageBox::question(this, " ", tr("Данное произведение будет удалено из избранного.\nПродолжить удаление?"), QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes)
        {
            QSqlQuery q(QSqlDatabase::database(sDBAlias));
            QString sSql = "DELETE FROM favorites WHERE ID_user=%1 AND ID_favorites=%2";
            q.exec(sSql.arg(sIdUser).arg(twTrackFav->selectedItems().at(0)->text(0)));
            setTrackFav();
        }
    }
}
//----------------------------------------------------------                    //даблклик из обычного - прослушивание
void mwMain::on_twTrackPlay_itemDoubleClicked(QTreeWidgetItem *, int)
{
    if(player &&
            twTrackPlay->selectedItems().count())
    {
        player->setMedia(QUrl::fromLocalFile(twTrackPlay->selectedItems().at(0)->text(4)));
        hsTrack->setValue(0);                                                  //ползунок
        player->play();
    }
}
//----------------------------------------------------------                    //даблкник в понравивщемся
void mwMain::on_twTrackFav_itemDoubleClicked(QTreeWidgetItem *, int )
{
    if(twTrackFav->selectedItems().count())                                     //если выбрано даблклик
        pbPlay->click();
}
//----------------------------------------------------------                    //прослушать трек
void mwMain::on_pbPlay_clicked()
{
    if(player &&
            twTrackFav->selectedItems().count())
    {
        player->setMedia(QUrl::fromLocalFile(twTrackFav->selectedItems().at(0)->text(4)));
        hsTrack->setValue(0);                                                  //ползунок
        player->play();
    }
}
//----------------------------------------------------------
void mwMain::my_viewPos(qint64 pos)                                             //изменение позиции
{
    hsTrack->setValue(int(pos/1000));                                           //миллисекунды в секунды
}
//----------------------------------------------------------
void mwMain::my_viewSize(qint64 dur)                                            //размер трека
{
    hsTrack->setMaximum(int(dur/1000));
}
//----------------------------------------------------------                    //передвижение трека звучания
void mwMain::on_hsTrack_sliderReleased()
{
    player->setPosition(hsTrack->value()*1000);
}
//----------------------------------------------------------
//шапка
//----------------------------------------------------------
void mwMain::on_pbAbout_clicked()                                               //о программе
{
    dAbout *dialog = new dAbout(this);
    if(dialog)
    {
        dialog->exec();
        delete dialog;
    }
}
//----------------------------------------------------------
void mwMain::on_pbUser_clicked()                                                //сменить пользователя
{
    if(QMessageBox::question(this, " ", tr("Вы уверены, что хотите выйти из учетной записи?"), QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes)
    {
        if (player)
            pbStop->click();                                                    //остановить плеер
        pbTrClear->click();                                                     //отчистить выборку
        swMain->setCurrentWidget(pLogin);
        leLogin->clear();
        lePass->clear();
        leLogin->setFocus();
    }
}
//----------------------------------------------------------
void mwMain::on_pbClose_clicked()                                               //кнопка Выход
{
    if(QMessageBox::question(this, " ", tr("Завершить работу программы?"), QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes)
        close();
}
//----------------------------------------------------------
//начальное окно регистрации
//----------------------------------------------------------
void mwMain::on_pbLogin_clicked()                                               //кнопка Вход
{
    QSqlQuery q(QSqlDatabase::database(sDBAlias));
    QString sSql = "SELECT * FROM users WHERE login='%1' AND password='%2'";
    q.exec(sSql.arg(leLogin->text()).arg(lePass->text()));
    if(q.next())
    {
        sIdUser = q.value("ID_user").toString();
        lLogin->setText(q.value("login").toString());
        if(q.value("login").toString() == "admin")                              //если admin admin
        {
            swMain->setCurrentWidget(pAdmin);
            setExec();
            setStyle();
            setTrack();
        }
        else                                                                    //если любо пользователь
        {
            swMain->setCurrentWidget(pUser);
            setTrackPlay();
            setTrackFav();
            my_update();
        }
    }
    else
    {
        QMessageBox::question(this, " ", tr("Некорректные данные.\nВведите еще раз."), QMessageBox::Ok);
        leLogin->clear();
        lePass->clear();
        leLogin->setFocus();
    }
}
//----------------------------------------------------------                    //ентер чтобы войти
void mwMain::on_lePass_returnPressed()
{
    pbLogin->click();
}
//----------------------------------------------------------
void mwMain::on_pbReg_clicked()                                                 //кнопка зарегистироваться
{
    dReg *dialog = new dReg(sDBAlias, this);
    if(dialog)
    {
        dialog->exec();
        leLogin->clear();
        lePass->clear();
        leLogin->setFocus();
        delete dialog;
    }
}
//----------------------------------------------------------
//заполнение от лица администратора
//----------------------------------------------------------
void mwMain::setExec()                                                          //заполнение исполнителей
{
    twExec->clear();
    QSqlQuery q(QSqlDatabase::database(sDBAlias));
    QString sSql = "SELECT * FROM executor";
    q.exec(sSql);
    while(q.next())
    {
        QTreeWidgetItem *trwi = new QTreeWidgetItem(twExec);
        if(trwi)
        {
            trwi->setText(0, q.value("ID_executor").toString());
            trwi->setText(1, QString("%1 %2 %3")
                          .arg(q.value("surname").toString())
                          .arg(q.value("name").toString())
                          .arg(q.value("twoname").toString()));
        }
    }
}
//----------------------------------------------------------
void mwMain::setStyle()                                                         //заполнение стилей
{
    twStyle->clear();
    QSqlQuery q(QSqlDatabase::database(sDBAlias));
    QString sSql = "SELECT * FROM styles";
    q.exec(sSql);
    while(q.next())
    {
        QTreeWidgetItem *trwi = new QTreeWidgetItem(twStyle);
        if(trwi)
        {
            trwi->setText(0, q.value("ID_style").toString());
            trwi->setText(1, q.value("title").toString());
            trwi->setText(2, q.value("about").toString());
        }
    }
}
//----------------------------------------------------------
void mwMain::setTrack()                                                         //заполнение треков
{
    twTrack->clear();
    QSqlQuery q(QSqlDatabase::database(sDBAlias));
    QString sSql = "SELECT tr.ID_track, tr.name,"
                   "ex.name  as ex_name, ex.surname, ex.twoname, "
                   "st.title, tr.path "
                   "FROM track as tr, executor as ex, styles as st "
                   "WHERE ex.ID_executor=tr.ID_executor AND st.ID_style=tr.ID_style";
    q.exec(sSql);
    while(q.next())
    {   QTreeWidgetItem *trwi = new QTreeWidgetItem(twTrack);
        if(trwi)
        {
            trwi->setText(0, q.value("ID_track").toString());
            trwi->setText(1, q.value("name").toString());
            trwi->setText(2, QString("%1\n%2 %3").arg(q.value("surname").toString()).arg(q.value("ex_name").toString()).arg(q.value("twoname").toString()));
            trwi->setText(3, q.value("title").toString());
            trwi->setText(4, q.value("path").toString());
        }
    }
}
//----------------------------------------------------------
//от лица админа добавление\изменение\удаление исполнителя
//----------------------------------------------------------
void mwMain::on_pbAddExec_clicked()                                             //кнопка добавить исполнителя
{
    dExec *dialog = new dExec(sDBAlias, "", this);
    if(dialog)
    {
        dialog->exec();
        delete dialog;
        setExec();
    }
}
//----------------------------------------------------------
void mwMain::on_pbEgitExec_clicked()                                            //кнопка изменить исполнителя
{
    if(twExec->selectedItems().count())
    {
        dExec *dialog = new dExec (sDBAlias, twExec->selectedItems().at(0)->text(0), this);
        if(dialog)
        {
            dialog->exec();
            delete dialog;
            setExec();
            setTrack();                                                         //обновить и треки
        }
    }
}
//----------------------------------------------------------                    //даблклик исполнителя
void mwMain::on_twExec_itemDoubleClicked(QTreeWidgetItem *, int )
{
    if(twExec->selectedItems().count())
        pbEgitExec->click();
}
//----------------------------------------------------------                    //кнопка удалить исполнителя
void mwMain::on_pbDelExec_clicked()
{   if(QMessageBox::question(this, " ", tr("Будут удалены все связанные записи.\nПродолжить удаление?"), QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes)
    {
        if(twExec->selectedItems().count())
        {
            QSqlQuery q(QSqlDatabase::database(sDBAlias));
            QString sSql = "DELETE FROM executor WHERE ID_executor=%1";
            q.exec(sSql.arg(twExec->selectedItems().at(0)->text(0)));
            setExec();
            setTrack();                                                         //обновить и треки
        }
    }
}
//----------------------------------------------------------
//от лица админа добавление\изменение\удаление стиля
//----------------------------------------------------------
void mwMain::on_pbAddStyle_clicked()                                            //кнопка добавить стиль
{
    dStyle *dialog = new dStyle(sDBAlias, "", this);
    if(dialog)
    {
        dialog->exec();
        delete dialog;
        setStyle();
    }
}
//----------------------------------------------------------
void mwMain::on_pbEgitStyle_clicked()                                           //кнопка изменить стиль
{
    if(twStyle->selectedItems().count())
    {
        dStyle *dialog = new dStyle(sDBAlias, twStyle->selectedItems().at(0)->text(0), this);
        if(dialog)
        {
            dialog->exec();
            delete dialog;
            setStyle();
            setTrack();                                                         //обновить и треки
        }
    }
}
//----------------------------------------------------------                    //даблклик стиля
void mwMain::on_twStyle_itemDoubleClicked(QTreeWidgetItem *, int )
{
    if(twStyle->selectedItems().count())
        pbEgitStyle->click();
}
//----------------------------------------------------------                    //кнопка удалить стиль
void mwMain::on_pbDelStyle_clicked()
{
    if(QMessageBox::question(this, " ", tr("Будут удалены все связанные записи.\nПродолжить удаление?"), QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes)
    {
        if(twStyle->selectedItems().count())
        {   QSqlQuery q(QSqlDatabase::database(sDBAlias));
            QString sSql = "DELETE FROM styles WHERE ID_style=%1";
            q.exec(sSql.arg(twStyle->selectedItems().at(0)->text(0)));
            setStyle();
            setTrack();                                                         //обновить и треки
        }
    }
}
//----------------------------------------------------------
//от лица админа добавление\изменение\удаление трека
//----------------------------------------------------------                    //кнопка добавить трек
void mwMain::on_pbAddTrack_clicked()
{
    dTrack *dialog = new dTrack(sDBAlias, "", this);
    if(dialog)
    {
        dialog->exec();
        delete dialog;
        setTrack();
    }
}
//----------------------------------------------------------                    //кнопка изменить трек
void mwMain::on_pbEgitTrack_clicked()
{
    if(twTrack->selectedItems().count())
    {
        dTrack *dialog = new dTrack(sDBAlias, twTrack->selectedItems().at(0)->text(0), this);
        if(dialog)
        {
            dialog->exec();
            delete dialog;
            setTrack();
        }   }
}
//----------------------------------------------------------                    //даблклик трека
void mwMain::on_twTrack_itemDoubleClicked(QTreeWidgetItem *, int )
{
    if(twTrack->selectedItems().count())
        pbEgitTrack->click();
}
//----------------------------------------------------------                    //кнопка удалить трек
void mwMain::on_pbDelTrack_clicked()
{
    if(QMessageBox::question(this, " ", tr("Запись будет безвозвратно удалена.\nПродолжить удаление?"), QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes)
    {
        if(twTrack->selectedItems().count())
        {
            QSqlQuery q(QSqlDatabase::database(sDBAlias));
            QString sSql = "DELETE FROM track WHERE ID_track=%1";
            q.exec(sSql.arg(twTrack->selectedItems().at(0)->text(0)));
            setTrack();
        }
    }
}
//----------------------------------------------------------
//убрать возможность удаления и изменения
//----------------------------------------------------------                    //исполнителя у админа
void mwMain::on_twExec_itemSelectionChanged()
{
    pbDelExec->setEnabled(twExec->selectedItems().count());
    pbEgitExec->setEnabled(twExec->selectedItems().count());
}
//----------------------------------------------------------                    //стиля у админа
void mwMain::on_twStyle_itemSelectionChanged()
{
    pbDelStyle->setEnabled(twStyle->selectedItems().count());
    pbEgitStyle->setEnabled(twStyle->selectedItems().count());
}
//----------------------------------------------------------                    //трека у админа
void mwMain::on_twTrack_itemSelectionChanged()
{
    pbDelTrack->setEnabled(twTrack->selectedItems().count());
    pbEgitTrack->setEnabled(twTrack->selectedItems().count());
}
//----------------------------------------------------------                    //статистика
void mwMain::on_pbGrafic_clicked()
{
     dGrafic *dialog = new dGrafic(sDBAlias, this);
    if(dialog)
    {
        dialog->exec();
        delete dialog;
    }
}
//----------------------------------------------------------
