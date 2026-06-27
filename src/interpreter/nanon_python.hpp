#pragma once

#include <string>


struct ExecutionResult {
    bool success;
    std::string stdout;
    std::string stderr;
};


class NanonPythonInterpreter
{
public:
    NanonPythonInterpreter();
    virtual ~NanonPythonInterpreter();

    bool start();
    ExecutionResult executeCode(std::string& code);

private:
    pid_t pid;
    int writeFd;
    int readFd;
};