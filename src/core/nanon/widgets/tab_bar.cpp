
#include "nanon/widgets/tab_bar.hpp"

#include <QBrush>
#include <QStyle>
#include <QStyleOptionTab>
#include <QStylePainter>


using namespace nanon::widgets;


NanonTabWidget::NanonTabWidget(QWidget *parent)
    : QTabWidget(parent)
{
    auto tabBar = new NanonTabBar();
    tabBar->setAutoFillBackground(true);
    setTabBar(tabBar);
}


void NanonTabBar::paintEvent(QPaintEvent *event)
{
    QStylePainter painter(this);
    QStyleOptionTab opt;

    QColor highlight = palette().color(QPalette::Highlight);
    QColor button = palette().color(QPalette::Button);

    painter.setRenderHint(QPainter::Antialiasing);

    for (int i = 0; i < count(); ++i) {
        initStyleOption(&opt, i);

        QRect tabRect = opt.rect;
        QPoint center = tabRect.center();

        QPoint first = QPoint(tabRect.x(), center.y());
        QPoint second;
        if (i == count() - 1) {
            second = QPoint(center.x(), center.y());
        } else {
            second = QPoint(tabRect.x() + tabRect.width(), center.y());
        }

        painter.setPen(button);
        painter.drawLine(first, second);

        int circleDiameter;
        if (i == currentIndex()) {
            circleDiameter = tabRect.height() - 2;
            QBrush currentBrush = QBrush(highlight);
            painter.setBrush(currentBrush);
        } else {
            circleDiameter = tabRect.height() - 6;
            QBrush otherBrush = QBrush(button);
            painter.setBrush(otherBrush);
        }
        int circleRadius = circleDiameter / 2;
        QRect circleRect = QRect(center.x() - circleRadius, center.y() - circleRadius, circleDiameter, circleDiameter);

        painter.drawEllipse(circleRect);

        if (i == currentIndex()) {

            QRect innerRect = QRect(circleRect.x() + 4, circleRect.y() + 4, circleRect.width() - 8, circleRect.height() - 8);
            painter.drawEllipse(innerRect);
        }

    }
}


QSize NanonTabBar::tabSizeHint(int index) const
{
    return QSize(30, 13);
}
