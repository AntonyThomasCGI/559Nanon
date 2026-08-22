#pragma once

#include "nanon/commands/action.hpp"
#include "nanon/session.hpp"

#include <QList>
#include <QObject>
#include <QSharedPointer>
#include <QSortFilterProxyModel>
#include <QString>
#include <QtGui/QStandardItemModel>
#include <QtWidgets/QListView>
#include <QtWidgets/QMenu>


namespace nanon {
namespace widgets {


/** A custom filter to sort recently executed actions at the top. */
class CommonActionsFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    CommonActionsFilterProxyModel(QList<QString> recentActions, QObject *parent = nullptr);

    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;

private:
    QList<QString> m_recentActions;
};


/** A generic menu for running actions. */
class NanonMenu : public QMenu
{
    Q_OBJECT

public:
    NanonMenu(QMenu *parent = nullptr);
    NanonMenu(QList<commands::NanonAction*> &actions, QMenu *parent = nullptr);
    NanonMenu(QList<commands::NanonAction*> &actions, QSharedPointer<NanonSession> session, QMenu *parent = nullptr);

protected:
    CommonActionsFilterProxyModel* m_proxyModel;
    QListView* m_view;
    QSharedPointer<NanonSession> m_session;

    void setupActions(QList<commands::NanonAction*> &actions);
    void configurePalette();
    void runAction(const QModelIndex &index);
};



}  // namespace widgets
}  // namespace nanon
