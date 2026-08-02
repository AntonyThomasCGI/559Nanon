#pragma once

#include "<nanon/commands/command.hpp>"

#include <memory>
#include <functional>

namespace nanon {
namespace commands {


class NanonCommandRegistry
{

public:
    NanonCommandRegistry(const NanonCommandRegistry&) = delete;
    NanonCommandRegistry& operator=(const NanonCommandRegistry&) = delete;
    NanonCommandRegistry(NanonCommandRegistry&&) = delete;
    NanonCommandRegistry& operator=(NanonCommandRegistry&&) = delete;

    static NanonCommandRegistry& instance() {
        static NanonCommandRegistry instance;
        return instance;
    }

    void registerCommand(const QString &commandName, NanonCommand *command)
    {
        m_commands[commandName] = command;
    };

private:
    NanonCommandRegistry() {};
    ~NanonCommandRegistry() = default;

    QMap<QString, NanonCommand*> m_commands;
};

}
}