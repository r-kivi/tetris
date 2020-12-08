/* Tetris project: main.cpp
 *
 * Main function
 *
 * Program author/editor:
 * Name: Rasmus Kivinen
 * Student number: 285870
 * UserID: kivinenr
 * E-Mail: rasmus.kivinen@tuni.fi
 * */

#include "mainwindow.hh"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
