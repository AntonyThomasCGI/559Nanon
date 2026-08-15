#pragma once

#include <QSize>
#include <QtWidgets/QTabBar>
#include <QtWidgets/QTabWidget>


namespace nanon {
namespace widgets {


class NanonTabWidget : public QTabWidget
{
    Q_OBJECT

public:

    NanonTabWidget(QWidget *parent = nullptr);
    virtual ~NanonTabWidget() = default;
};


class NanonTabBar : public QTabBar
{
    Q_OBJECT

protected:
    void paintEvent(QPaintEvent *event) override;
    QSize tabSizeHint(int index) const override;
};


}  // namespace widgets
}  // namespace nanon
