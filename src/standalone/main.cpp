#include "nanon.hpp"
#include "interpreters/nanon_python.hpp"

#include <QtWidgets/QApplication>

#include <iostream>
#include <string>


int main(int argc, char *argv[])
{
    std::cout << "Launching Nanon" << std::endl;

    NanonPythonInterpreter *interpreter = new NanonPythonInterpreter();

    interpreter->start();

    QApplication app(argc, argv);
    NanonWindow nanon;
    nanon.resize(800, 600);

    nanon.setInterpreter(interpreter);

    nanon.show();


    return app.exec();
}

