
#include <QtWidgets/QApplication>
#include "nanon.hpp"

#include <iostream>
#include <windows.h>

int main(int argc, char *argv[])
{
    AllocConsole();
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);

    QApplication NanonApp(argc, argv);
    NanonWindow nanon;
    nanon.show();

    return NanonApp.exec();
}
