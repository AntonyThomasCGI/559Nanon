

#include "nanon/commands/registry.hpp"


using namespace nanon::commands;


void NanonCommandRegistry::registerAction(NanonAction *action)
{
    if (m_actions.contains(action->text())) {
        qWarning("Action with name '%s' is already registered.", action->text().toStdString().c_str());
        return;
    }
    if (action->text().isEmpty()) {
        qWarning("Action name cannot be empty.");
        return;
    }
    m_actions[action->text()] = action;
};

void NanonCommandRegistry::unregisterAction(NanonAction *action)
{
    if (!m_actions.contains(action->text())) {
        qWarning("Action with name '%s' is not registered.", action->text().toStdString().c_str());
        return;
    }
    m_actions.remove(action->text());
};

QList<NanonAction*> NanonCommandRegistry::getAllActions(bool hidden)
{
    if (hidden) {
        return m_actions.values();
    }
    QList<NanonAction*> visibleActions;
    for (auto *action : m_actions.values()) {
        if (!action->isHidden()) {
            visibleActions.append(action);
        }
    }
    return visibleActions;
}
