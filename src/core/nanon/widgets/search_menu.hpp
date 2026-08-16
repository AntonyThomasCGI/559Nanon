#pragma once

#include "nanon/commands/action.hpp"
#include "nanon/widgets/menu.hpp"

#include <QAction>
#include <QEvent>
#include <QList>
#include <QtWidgets/QLineEdit>


namespace nanon {
namespace widgets {


/**
 * A menu with a search bar for filtering the action items.
 */
class SearchMenu : public NanonMenu
{
    Q_OBJECT

public:
    SearchMenu(QList<commands::NanonAction*> &actions, QMenu *parent = 0);

protected:
    bool eventFilter(QObject *watched, QEvent *event);

    void configurePalette();

private:
    void moveSelection(int count);
    void onSearchTextChanged(const QString &text);

    QLineEdit* m_searchBar;
};



}  // namespace widgets
}  // namespace nanon

