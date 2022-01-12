#include "dgrafic.h"
#include <QtSql>
#include <QtDebug>
//----------------------------------------------------------
dGrafic::dGrafic(QString ALIAS,QWidget *parent) : QDialog(parent)
{
  setupUi(this);
    sDBAlias = ALIAS;                                                           // подклюение к БД
    QSqlQuery q(QSqlDatabase::database(sDBAlias));                              // запрос на кол-во песен
    QString sSql = "select count(tr.id_executor) as cou "
                       "FROM track as tr, executor as ex "
                       "WHERE ex.id_executor=tr.id_executor "
                       "GROUP BY ex.surname "
                       "ORDER BY cou DESC LIMIT 1";
    q.exec(sSql);
    int iYmax = 1;
    if(q.next())
        iYmax = q.value("cou").toInt();                                         // максимальное количество треков
    sSql = "select ex.surname, count(tr.id_executor) "                          // авторы, кол-во песен
                      "FROM track as tr, executor as ex "
                      "WHERE ex.id_executor = tr.id_executor "
                      "GROUP BY ex.surname";
    q.exec(sSql);
    QBarSet *set0 = new QBarSet(" ");                                           // легента

    QStringList categories;
    while(q.next())
    {
        *set0 << q.value("count").toInt();                                      // по у
        categories << q.value("surname").toString();                            // по х
    }

    QBarSeries *series = new QBarSeries();                                      // тип графика
    series->append(set0);
    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("График по авторам и количеству песен");
    chart->setAnimationOptions(QChart::SeriesAnimations);                       // анимация

    QBarCategoryAxis *axisX = new QBarCategoryAxis();                           // по х, снизу, добавляем
    axisX->append(categories);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();                                       // по у, по левому, добавляем
    axisY->setRange(0, iYmax);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    chart->legend()->setVisible(false);                                         // не нужна легенда

    QChartView *chartView = new QChartView(chart);                              // создаем видимое и заполняем
    chartView->setRenderHint(QPainter::Antialiasing);                           // сглаживание

    vLayout->addWidget(chartView);                                              // отображаем
}

//----------------------------------------------------------
void dGrafic::on_pbOk_clicked()
{
    accept();
}
//----------------------------------------------------------
