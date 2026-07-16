
#include "nanon_maya_interpreter.hpp"

#include <maya/MGlobal.h>



bool NanonMayaInterpreter::start()
{
    return true;
}


ExecutionResult NanonMayaInterpreter::executeCode(std::string& code)
{
    MString command(code.c_str());
    MString scriptOutput;
    MStatus status = MGlobal::executePythonCommand(command, scriptOutput);

    return ExecutionResult{!status.error(), scriptOutput.asChar()};
}
