#pragma once

#include "nanon/session.hpp"

#include <QtGui/QStandardItemModel>
#include <QtWidgets/QMenu>



namespace nanon {
namespace widgets {


class SearchMenu : public QMenu
{
    Q_OBJECT

public:
    SearchMenu(QStandardItemModel *actionModel, QMenu *parent = 0);

private:
    void onSearchTextChanged(const QString &text);
};



}  // namespace widgets
}  // namespace nanon

