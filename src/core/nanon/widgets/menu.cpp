
#include "nanon/style/theme_manager.hpp"
#include "nanon/widgets/menu.hpp"

#include <QPalette>
#include <QtWidgets/QWidgetAction>


using namespace nanon::widgets;


NanonMenu::NanonMenu(QMenu *parent)
    : QMenu(parent) {}


NanonMenu::NanonMenu(QList<commands::NanonAction*> &actions, QMenu *parent)
    : QMenu(parent)
{
    setupActions(actions);
}

NanonMenu::NanonMenu(QList<commands::NanonAction*> &actions, QSharedPointer<NanonSession> session, QMenu *parent)
    : QMenu(parent), m_session(session)
{
    setupActions(actions);
}


void NanonMenu::setupActions(QList<commands::NanonAction*> &actions)
{
    // Create the item model
    QList<QStandardItem*> items;
    for (auto &action : actions) {
        QStandardItem *item = new QStandardItem(action->text());
        item->setData(QVariant::fromValue(action), Qt::UserRole);
        items.push_back(item);
    }
    QStandardItemModel *actionModel = new QStandardItemModel();
    actionModel->appendColumn(items);

    QList<QString> recentActions;
    if (m_session) {
        recentActions = m_session->getRecentActions();
    }

    m_proxyModel = new CommonActionsFilterProxyModel(recentActions, this);
    m_proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    m_proxyModel->setSourceModel(actionModel);
    m_proxyModel->sort(0, Qt::AscendingOrder);
    m_proxyModel->setDynamicSortFilter(true);


    m_view = new QListView(this);
    m_view->setModel(m_proxyModel);
    m_view->setFrameShape(QFrame::NoFrame);
    connect(m_view, &QListView::clicked, this, &NanonMenu::runAction);

    QModelIndex firstIndex = m_proxyModel->index(0, 0);
    if (firstIndex.isValid()) {
        m_view->setCurrentIndex(firstIndex);
    }

    QWidgetAction *actionViewAction = new QWidgetAction(this);
    actionViewAction->setDefaultWidget(m_view);
    addAction(actionViewAction);
}


void NanonMenu::configurePalette()
{
    // Styling
    QPalette palette;
    auto theme = style::NanonThemeManager::instance().theme();

    palette.setColor(
        QPalette::Base,
        QColor(theme->getColor("menu.background"))
    );
    palette.setColor(
        QPalette::Window,
        QColor(theme->getColor("menu.background"))
    );
    palette.setColor(
        QPalette::Text,
        QColor(theme->getColor("menu.foreground"))
    );
    palette.setColor(
        QPalette::WindowText,
        QColor(theme->getColor("menu.foreground"))
    );
    palette.setColor(
        QPalette::Highlight,
        QColor(theme->getColor("menu.active.background"))
    );
    palette.setColor(
        QPalette::HighlightedText,
        QColor(theme->getColor("menu.active.foreground"))
    );

    setPalette(palette);
}


void NanonMenu::runAction(const QModelIndex &index)
{
    QVariant actionVariant = m_proxyModel->data(index, Qt::UserRole);
    if (actionVariant.canConvert<QAction*>()) {
        QAction *action = actionVariant.value<QAction*>();
        if (action) {
            if (m_session) {
                m_session->saveRecentAction(action->text());
            }
            this->close();
            action->trigger();
        }
    }
}


CommonActionsFilterProxyModel::CommonActionsFilterProxyModel(QList<QString> recentActions, QObject *parent)
    : QSortFilterProxyModel(parent), m_recentActions(recentActions)
{}


bool CommonActionsFilterProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    QVariant leftActionVariant = sourceModel()->data(left, Qt::UserRole);
    QVariant rightActionVariant = sourceModel()->data(right, Qt::UserRole);

    if (!leftActionVariant.canConvert<QAction*>() || !rightActionVariant.canConvert<QAction*>()) {
        qWarning() << "Can not convert menu item to action!";
        return false;
    }

    QAction *leftAction = leftActionVariant.value<QAction*>();
    QAction *rightAction = rightActionVariant.value<QAction*>();

    int leftIndex = m_recentActions.indexOf(leftAction->text());
    int rightIndex = m_recentActions.indexOf(rightAction->text());

    if (leftIndex == -1 && rightIndex == -1) {
        return QString::localeAwareCompare(leftAction->text(), rightAction->text());
    }
    else if (leftIndex == -1) {
        return false;
    }
    else if (rightIndex == -1) {
        return true;
    }

    return leftIndex < rightIndex;
}
