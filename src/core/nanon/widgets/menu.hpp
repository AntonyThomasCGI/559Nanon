#pragma once

#include "nanon/commands/action.hpp"

#include <QList>
#include <QObject>
#include <QSortFilterProxyModel>
#include <QtGui/QStandardItemModel>
#include <QtWidgets/QListView>
#include <QtWidgets/QMenu>


namespace nanon {
namespace widgets {


class NanonMenu : public QMenu
{
    Q_OBJECT

public:
    NanonMenu(QMenu *parent = nullptr);
    NanonMenu(QList<commands::NanonAction*> &actions, QMenu *parent = nullptr);

protected:
    void setupActions(QList<commands::NanonAction*> &actions);
    void configurePalette();

    QListView* m_view;
    QSortFilterProxyModel* m_model;

    void runAction(const QModelIndex &index);
};



}  // namespace widgets
}  // namespace nanon
