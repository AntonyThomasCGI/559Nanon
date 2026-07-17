#pragma once

#include "nanon/interpreter/base.hpp"


/**
 * An interpreter which executes python code in maya.
 */
class NanonMayaInterpreter : public nanon::interpreter::NanonInterpreterBase
{
public:

    bool start() override;
    nanon::interpreter::ExecutionResult executeCode(std::string& code) override;
};
