

#include "nanon_python.hpp"
#include "../io/nanon_socket.hpp"

#include <iostream>
#include <unistd.h>

#include <nlohmann/json.hpp>


NanonPythonInterpreter::NanonPythonInterpreter()
{

}


NanonPythonInterpreter::~NanonPythonInterpreter()
{

}


bool NanonPythonInterpreter::start()
{
    pid = fork();

    if (pid < 0) {
        return false;
    }

    if (pid == 0)
    {
        // Child process

        // Replace this process with Python kernel
        execlp(
            "python3",
            "python3",
            "-u",
            "python/_nanon/kernel.py",
            nullptr
        );

        // If exec fails
        _exit(1);
    }

    return true;
}


ExecutionResult NanonPythonInterpreter::executeCode(std::string& code)
{
    NanonSocket socket;
    bool result = socket.sendJson(code);

    uint32_t len;
    socket.readAll(&len, 4);
    len = ntohl(len);

    std::string payload(len, '\0');
    socket.readAll(payload.data(), len);

    auto json = nlohmann::json::parse(payload);

    std::cout << "Got data back c++" << std::endl;
    std::cout << json["stdout"] << std::endl;
    std::cout << json["stderr"] << std::endl;


    return ExecutionResult{1, "todo", "todo"};
}
