#pragma once

#include "nanon/session.hpp"

#include <QShortcut>
#include <QtGui/QAction>
#include <QtGui/QKeySequence>


namespace nanon {
namespace commands {


// Forward declaration
class NanonCommandRegistry;


/**
 * An action that is automatically registered with the NanonCommandRegistry and
 * can be triggered by a keyboard shortcut.
 */
class NanonAction : public QAction
{

public:

    NanonAction(const QString &text, QObject *parent, bool hidden = false);
    NanonAction(const QString &text, const QKeySequence &key, QObject *parent, bool hidden = false);
    virtual ~NanonAction();

    bool isHidden() const { return m_hidden; }

private:
    bool m_hidden;

};


}
}
