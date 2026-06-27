
#include "nanon.hpp"
#include "interpreter/nanon_python.hpp"

#include <iostream>
#include <string>
#include <thread>
#include <chrono>


#include <QtWidgets/QApplication>


int main(int argc, char *argv[])
{
    std::cout << "Hello, Nanon!" << std::endl;

    NanonPythonInterpreter interpreter;

    std::cout << "Starting interpreter" << std::endl;
    interpreter.start();

    std::this_thread::sleep_for(std::chrono::seconds(1));

    std::string code = "print('hi from python')";

    std::cout << "Executing code: " << code << std::endl;

    interpreter.executeCode(code);

    std::this_thread::sleep_for(std::chrono::seconds(1));

    std::string err_code = "print('yep')\nraise AssertionError('die potato die')";
    interpreter.executeCode(err_code);

    std::this_thread::sleep_for(std::chrono::seconds(1));

    std::cout << "Exit" << std::endl;

    //QApplication app(argc, argv);
    //NanonWindow nanon;

    //nanon.show();

    //return app.exec();
    return 0;
}
