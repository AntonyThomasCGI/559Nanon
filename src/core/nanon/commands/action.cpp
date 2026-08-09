
#include "nanon/commands/action.hpp"
#include "nanon/commands/registry.hpp"


using namespace nanon::commands;


NanonAction::NanonAction(const QString &text, QObject *parent, bool hidden)
    : QAction(text, parent), m_hidden(hidden)
{
    NanonCommandRegistry::instance().registerAction(this);
}

NanonAction::NanonAction(const QString &text, const QKeySequence &key, QObject *parent, bool hidden)
    : QAction(text, parent), m_hidden(hidden)
{
    QShortcut *shortcut = new QShortcut(key, parent);
    connect(shortcut, &QShortcut::activated, this, &NanonAction::trigger);

    NanonCommandRegistry::instance().registerAction(this);
}


NanonAction::~NanonAction()
{
    NanonCommandRegistry::instance().unregisterAction(this);
};