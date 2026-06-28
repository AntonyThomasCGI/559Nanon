
#include "nanon.hpp"
#include "interpreter/nanon_python.hpp"

#include <iostream>
#include <string>


#include <QtWidgets/QApplication>


int main(int argc, char *argv[])
{
    std::cout << "Hello, Nanon!" << std::endl;

    NanonPythonInterpreter *interpreter = new NanonPythonInterpreter();

    std::cout << "Starting interpreter" << std::endl;
    interpreter->start();

    QApplication app(argc, argv);
    NanonWindow nanon;

    nanon.setInterpreter(interpreter);

    nanon.show();

    return app.exec();
}
