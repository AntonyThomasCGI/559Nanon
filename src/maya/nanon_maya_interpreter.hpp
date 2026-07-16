#pragma once

#include "interpreters/nanon_interpreter.hpp"


class NanonMayaInterpreter : public NanonInterpreterBase
{
public:

    bool start() override;
    ExecutionResult executeCode(std::string& code) override;
};
