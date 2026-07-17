#include "nanon/window.hpp"
#include "nanon/interpreter/python.hpp"

#include <QtWidgets/QApplication>

#include <iostream>
#include <string>


int main(int argc, char *argv[])
{
    std::cout << "Launching Nanon" << std::endl;

    auto *interpreter = new nanon::interpreter::NanonPythonInterpreter();

    interpreter->start();

    QApplication app(argc, argv);
    nanon::NanonWindow nanon;
    nanon.resize(800, 600);

    nanon.setInterpreter(interpreter);

    nanon.show();


    return app.exec();
}

