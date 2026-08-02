#pragma once

#include "nanon/style/theme.hpp"

#include <QtCore/QObject>


namespace nanon {


class NanonSession : public QObject
{
    Q_OBJECT

public:
    NanonSession();
    ~NanonSession() = default;

    style::NanonTheme* currentTheme();
    void setCurrentTheme(style::NanonTheme *theme);

signals:
    void themeChanged(style::NanonTheme*);

};


}  // namespace nanon
