#pragma once

#include "nanon/style/theme.hpp"

#include "QtCore/QMap"
#include "QtCore/QString"


namespace nanon {
namespace style {

class NanonThemeManager
{
public:
    NanonThemeManager(const NanonThemeManager&) = delete;
    NanonThemeManager& operator=(const NanonThemeManager&) = delete;
    NanonThemeManager(NanonThemeManager&&) = delete;
    NanonThemeManager& operator=(NanonThemeManager&&) = delete;

    static NanonThemeManager& instance() {
        static NanonThemeManager instance;
        return instance;
    }

    NanonTheme* theme();
    void setTheme(NanonTheme *theme);
    void setThemeByName(QString themeName);

    void addTheme(QString name, QString path);


private:
    NanonThemeManager() {};
    ~NanonThemeManager() = default;

    NanonTheme *m_theme;
    QMap<QString, NanonTheme> m_availableThemes;
};

}
}
