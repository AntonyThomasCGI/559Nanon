#pragma once

#include "nanon_interpreter_base.hpp"

#include <string>


static const char* SOCKET_PATH = "/tmp/nanon.sock";


class NanonPythonInterpreter : public NanonInterpreterBase
{
public:
    NanonPythonInterpreter();
    virtual ~NanonPythonInterpreter();

    bool start() override;
    ExecutionResult executeCode(std::string& code) override;

private:
    pid_t pid;
    int writeFd;
    int readFd;

};