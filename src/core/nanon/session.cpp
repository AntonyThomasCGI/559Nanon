

#include "nanon/session.hpp"
#include "nanon/style/theme_manager.hpp"



#ifndef RESOURCE_PATH
#define RESOURCE_PATH = ""
#endif


using namespace nanon;


NanonSession::NanonSession()
{
    // Initialize the theme manager and load available themes
    auto& themeManager = style::NanonThemeManager::instance();
    themeManager.collectAvailableThemes(RESOURCE_PATH "/themes");

    m_settings = std::make_unique<QSettings>("Nanon", "Nanon");
}


style::NanonTheme* NanonSession::currentTheme()
{
    auto& themeManager = style::NanonThemeManager::instance();
    return themeManager.theme();
}


void NanonSession::setCurrentTheme(style::NanonTheme *theme)
{
    auto& themeManager = style::NanonThemeManager::instance();
    themeManager.setTheme(theme);

    emit themeChanged(theme);
}


void NanonSession::saveEditorContent(const QString &editorText)
{
    m_settings->setValue("session/editor/documents/0", editorText);
}


QString NanonSession::loadEditorContent()
{
    return m_settings->value("session/editor/documents/0").toString();
}
