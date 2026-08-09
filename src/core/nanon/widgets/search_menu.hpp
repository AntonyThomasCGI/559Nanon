#pragma once

#include "nanon/commands/action.hpp"

#include <QAction>
#include <QEvent>
#include <QList>
#include <QObject>
#include <QSortFilterProxyModel>
#include <QtGui/QStandardItemModel>
#include <QtWidgets/QListView>
#include <QtWidgets/QMenu>


namespace nanon {
namespace widgets {


/**
 * A menu with a search bar for filtering the action items.
 */
class SearchMenu : public QMenu
{
    Q_OBJECT

public:
    SearchMenu(QList<commands::NanonAction*> &actions, QMenu *parent = 0);

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private:
    void moveSelection(int count);
    void onSearchTextChanged(const QString &text);
    void runAction(const QModelIndex &index);

    QLineEdit* m_searchBar;
    QListView* m_view;
    QSortFilterProxyModel* m_model;
};



}  // namespace widgets
}  // namespace nanon

