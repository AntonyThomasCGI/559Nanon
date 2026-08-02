#pragma once

#include "nanon/style/theme.hpp"

#include "QtCore/QMap"
#include "QtCore/QString"

#include <filesystem>


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

    // TODO, any set methods here don't update session :thinking:

    NanonTheme* theme();
    NanonTheme* defaultTheme() { return &m_availableThemes["Solarized Light"]; };

    NanonTheme* getThemeByName(QString themeName) {
        if (!m_availableThemes.contains(themeName)) {
            return nullptr;
        }
        return &m_availableThemes[themeName];
    }

    void setTheme(NanonTheme *theme);
    void setThemeByName(QString themeName);

    void loadTheme(QString path);
    void collectAvailableThemes(std::filesystem::path themePath);


private:
    NanonThemeManager() {};
    ~NanonThemeManager() = default;

    NanonTheme *m_theme;
    QMap<QString, NanonTheme> m_availableThemes;
};

}
}
