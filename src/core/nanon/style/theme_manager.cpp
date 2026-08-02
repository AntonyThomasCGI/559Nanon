
#include "nanon/style/theme_manager.hpp"

#include <stdexcept>
#include <sstream>
#include <string>

#include <iostream>


using namespace nanon::style;


NanonTheme* NanonThemeManager::theme()
{
    return m_theme;
}


void NanonThemeManager::setTheme(NanonTheme *theme)
{
    m_theme = theme;
}


void NanonThemeManager::setThemeByName(QString themeName)
{
    if (!m_availableThemes.contains(themeName)) {
        std::stringstream err;
        err << "No such theme " << themeName.toStdString();
        throw std::runtime_error(err.str());
    }

    setTheme(&m_availableThemes[themeName]);
}


void NanonThemeManager::loadTheme(QString path)
{
    NanonTheme theme;
    theme.loadFromFile(path);
    m_availableThemes[theme.getName()] = theme;
}


void NanonThemeManager::collectAvailableThemes(std::filesystem::path themePath)
{
    if (!std::filesystem::exists(themePath)) {
        std::cerr << "Theme path does not exist: " << themePath.string() << std::endl;
        return;
    }

    for (const auto& entry : std::filesystem::directory_iterator(themePath)) {
        if (entry.is_regular_file() && entry.path().extension() == ".json") {
            loadTheme(QString::fromStdString(entry.path().string()));
        }
    }

    m_theme = defaultTheme();

}
