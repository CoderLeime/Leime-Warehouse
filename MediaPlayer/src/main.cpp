#include "widget.h"

#include <QApplication>
#include <QFile>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QFile qssFile("D:/QtProgram/MediaPlayer/Resources/QSS/widget.qss");
    if(qssFile.exists())
        qDebug()<<"";
    if(qssFile.open(QFile::ReadOnly)) {
        a.setStyleSheet(qssFile.readAll());
    }
    Widget w;
    w.show();
    return a.exec();
}
