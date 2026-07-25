
#include "nanon/style/theme_manager.hpp"

#include <stdexcept>
#include <sstream>
#include <string>


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


void NanonThemeManager::addTheme(QString name, QString path)
{
    NanonTheme& theme = m_availableThemes[name];
    theme.loadFromFile(path);
    if (m_theme == nullptr) {

        m_theme = &theme;
    }
}
