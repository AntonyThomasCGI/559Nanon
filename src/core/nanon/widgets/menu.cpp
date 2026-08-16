
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

    m_model = new QSortFilterProxyModel(this);
    m_model->setFilterCaseSensitivity(Qt::CaseInsensitive);
    m_model->setSourceModel(actionModel);

    m_view = new QListView(this);
    m_view->setModel(m_model);
    m_view->setFrameShape(QFrame::NoFrame);
    connect(m_view, &QListView::clicked, this, &NanonMenu::runAction);

    QModelIndex firstIndex = m_model->index(0, 0);
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
    // TODO, this assumes NanonActions were passed in as the model's data.
    QVariant actionVariant = m_model->data(index, Qt::UserRole);
    if (actionVariant.canConvert<QAction*>()) {
        QAction *action = actionVariant.value<QAction*>();
        if (action) {
            this->close();
            action->trigger();
        }
    }
}
