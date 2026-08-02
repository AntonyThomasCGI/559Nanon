#pragma once

#include "nanon/session.hpp"


namespace nanon {
namespace commands {


/**
 * Base class for all commands in Nanon.
 */
class NanonCommand
{

public:

    NanonCommand(NanonSession &session, const QString &name, QObject *parent = nullptr)
        : m_session(session), m_name(name), m_parent(parent)
    {}

    virtual ~NanonCommand() = default;

    virtual void execute() = 0;

private:
    NanonSession &m_session;
    QString m_name;
    QObject *m_parent;
};


}
}
