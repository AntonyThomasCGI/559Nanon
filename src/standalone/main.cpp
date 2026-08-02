#include "nanon/window.hpp"
#include "nanon/interpreter/python.hpp"

#include <QtWidgets/QApplication>

#include <iostream>
#include <memory>
#include <string>


int main(int argc, char *argv[])
{
    std::cout << "Welcome to Nanon" << std::endl;

    QApplication app(argc, argv);

    nanon::NanonWindow nanon;
    nanon.resize(800, 600);

    auto interpreter = std::make_unique<nanon::interpreter::NanonPythonInterpreter>();
    interpreter->start();
    nanon.setInterpreter(interpreter.get());

    nanon.show();

    return app.exec();
}

