

#include "nanon/interpreter/python.hpp"
#include "nanon/io/socket.hpp"

#include <chrono>
#include <iostream>
#include <sstream>
#include <thread>
#include <unistd.h>

#include <nlohmann/json.hpp>


using namespace nanon::interpreter;


NanonPythonInterpreter::NanonPythonInterpreter()
{

}


NanonPythonInterpreter::~NanonPythonInterpreter()
{

}


bool NanonPythonInterpreter::start()
{
    std::cout << "Starting python interpreter" << std::endl;

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
            "python/nanon/kernel.py",
            nullptr
        );

        // If exec fails
        _exit(1);
    }

    // Allow python process to start up
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    return true;
}


ExecutionResult NanonPythonInterpreter::executeCode(std::string& code)
{
    int socketFd = nanon::io::connectToSocket(SOCKET_PATH);

    bool result = nanon::io::sendJson(socketFd, code);
    if (!result) {
        std::cerr << "ERROR: Could not send code" << std::endl;
    }

    bool success = false;

    // Redirect everything to stdout, eventually it would be
    // nice to separate these.
    std::ostringstream sstdout;

    while (true) {
        uint32_t len;
        nanon::io::readAll(socketFd, &len, 4);
        len = ntohl(len);

        std::string payload(len, '\0');
        nanon::io::readAll(socketFd, const_cast<void*>(static_cast<const void*>(payload.data())), len);

        auto json = nlohmann::json::parse(payload);

        if (json["event"] == "stdout") {
            sstdout << json["text"].get<std::string>();
        } else if (json["event"] == "stderr") {
            sstdout << json["text"].get<std::string>();
        } else if (json["event"] == "repr") {
            sstdout << json["text"].get<std::string>();
        }   else if (json["event"] == "result") {
            success = json["success"];
            // Include the traceback in stderr
            break;
        }
    }

    std::string standardout = sstdout.str();

    return ExecutionResult{
        success,
        standardout,
        "",
    };
}
