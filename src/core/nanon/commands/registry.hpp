#pragma once

#include "nanon/commands/action.hpp"

#include <memory>
#include <functional>

namespace nanon {
namespace commands {


/**
 * Centralized registry of all Actions available in the UI.
 */
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

    void registerAction(NanonAction *action);
    void unregisterAction(NanonAction *action);

    QList<NanonAction*> getAllActions(bool hidden = false);

private:
    NanonCommandRegistry() {};
    ~NanonCommandRegistry() = default;

    QMap<QString, NanonAction*> m_actions;
};

}
}
