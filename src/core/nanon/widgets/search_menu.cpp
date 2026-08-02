
#include "nanon/style/theme_manager.hpp"
#include "nanon/widgets/search_menu.hpp"

#include <QAction>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidgetAction>

#include <iostream>


using namespace nanon::widgets;


SearchMenu::SearchMenu(QStandardItemModel *actionModel, QMenu *parent)
    : QMenu(parent)
{
    QLineEdit *lineEdit = new QLineEdit();

    QWidgetAction *searchAction = new QWidgetAction(this);
    searchAction->setDefaultWidget(lineEdit);

    addAction(searchAction);

    lineEdit->setFocus();

    //mainLayout->addWidget(lineEdit);
    connect(lineEdit, &QLineEdit::textChanged, this, &SearchMenu::onSearchTextChanged);

    QListView *actionView = new QListView();
    actionView->setModel(actionModel);

    connect(actionView, &QListView::clicked, [this, actionModel](const QModelIndex &index) {
        QVariant actionVariant = actionModel->data(index, Qt::UserRole);
        if (actionVariant.canConvert<QAction*>()) {
            QAction *action = actionVariant.value<QAction*>();
            if (action) {
                action->trigger();
                this->close();
            }
        }
    });

    QWidgetAction *actionViewAction = new QWidgetAction(this);
    actionViewAction->setDefaultWidget(actionView);

    addAction(actionViewAction);
}


void SearchMenu::onSearchTextChanged(const QString &text)
{
    std::cout << text.toStdString() << std::endl;
}
