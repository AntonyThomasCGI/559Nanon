
#pragma once

#include <string>


struct ExecutionResult {
    bool success;
    std::string stdout;
    std::string stderr;
};


class NanonInterpreterBase
{
public:
    NanonInterpreterBase() = default;
    virtual ~NanonInterpreterBase() = default;

    virtual bool start() = 0;

    virtual ExecutionResult executeCode(std::string& code) = 0;

};
