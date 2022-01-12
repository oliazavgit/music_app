#include "mwmain.h"
#include <QTextCodec>
#include <QTranslator>
#include <QDir>
#include <QApplication>
//----------------------------------------------------------
int main(int argc, char *argv[])
{
    QStringList paths = QCoreApplication::libraryPaths();                       //прописываем путь к плагинам
    paths.append(".");
    paths.append("plugins");
    paths.append("plugins/translations");
#ifdef Q_OS_WIN                                                                 // если Win
    paths.append("plugins/imageformats");
    paths.append("plugins/platforms");
    paths.append("plugins/audio");
    paths.append("plugins/sqldrivers");
    paths.append("plugins/imports");
//    paths.append("plugins/playlistformats");
#endif
    QCoreApplication::setLibraryPaths(paths);                                   //к стандартной бибиотеке добавляем свои
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

    QApplication a(argc, argv);
    QTranslator *translator = new QTranslator(&a);                              // устанавливаем руссификатор
    if(translator->load("qt_ru", QDir::toNativeSeparators(a.applicationDirPath()+"/translations")))
         a.installTranslator(translator);
    mwMain w;
    w.show();

    return a.exec();
}
//----------------------------------------------------------
