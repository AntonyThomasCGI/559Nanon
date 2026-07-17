#pragma once

#include "nanon/interpreter/base.hpp"


class NanonMayaInterpreter : public nanon::interpreter::NanonInterpreterBase
{
public:

    bool start() override;
    nanon::interpreter::ExecutionResult executeCode(std::string& code) override;
};
