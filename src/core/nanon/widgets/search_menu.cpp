
#include "nanon/style/theme_manager.hpp"
#include "nanon/widgets/search_menu.hpp"

#include <QAction>
#include <QFrame>
#include <QKeyEvent>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidgetAction>

#include <iostream>


using namespace nanon::widgets;


SearchMenu::SearchMenu(QList<commands::NanonAction*> &actions, QMenu *parent)
    : QMenu(parent)
{
    // Create the search bar
    m_searchBar = new QLineEdit(this);
    m_searchBar->installEventFilter(this);
    m_searchBar->setFrame(false);
    m_searchBar->setTextMargins(0, 0, 0, 0);
    connect(m_searchBar, &QLineEdit::textChanged, this, &SearchMenu::onSearchTextChanged);

    QWidgetAction *searchAction = new QWidgetAction(this);
    searchAction->setDefaultWidget(m_searchBar);
    addAction(searchAction);

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
    connect(m_view, &QListView::clicked, this, &SearchMenu::runAction);

    QModelIndex firstIndex = m_model->index(0, 0);
    if (firstIndex.isValid()) {
        m_view->setCurrentIndex(firstIndex);
    }

    QWidgetAction *actionViewAction = new QWidgetAction(this);
    actionViewAction->setDefaultWidget(m_view);
    addAction(actionViewAction);

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

    QPalette searchPalette;
    searchPalette.setColor(
        QPalette::Base,
        QColor(theme->getColor("input.background"))
    );
    m_searchBar->setPalette(searchPalette);

    // Disable focus highlight on Mac OS
    m_searchBar->setAttribute(Qt::WA_MacShowFocusRect, false);

    m_searchBar->setFocus();
}


bool SearchMenu::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_searchBar) {
        auto *keyEvent = static_cast<QKeyEvent *>(event);

        if (keyEvent->type() == event->KeyPress) {
            switch (keyEvent->key()) {
            case Qt::Key_Down:
                moveSelection(1);
                return true;

            case Qt::Key_Up:
                moveSelection(-1);
                return true;

            case Qt::Key_Return:
            case Qt::Key_Enter:
                runAction(m_view->currentIndex());
                return true;

            case Qt::Key_Escape:
                close();
                return true;
            }
        }
    }

    return QMenu::eventFilter(watched, event);
}


void SearchMenu::moveSelection(int count) {
    QModelIndex selection = m_view->selectionModel()->currentIndex();
    QModelIndex newIndex = m_model->index(selection.row() + count, selection.column());
    if (newIndex.isValid()) {
        m_view->setCurrentIndex(newIndex);
    }
}


void SearchMenu::onSearchTextChanged(const QString &text)
{
    m_model->setFilterFixedString(text);

    QModelIndex firstIndex = m_model->index(0, 0);
    if (firstIndex.isValid()) {
        m_view->setCurrentIndex(firstIndex);
    }
}


void SearchMenu::runAction(const QModelIndex &index)
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
