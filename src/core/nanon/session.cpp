

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


void NanonSession::saveEditorContent(int index, const QString &editorText)
{
    QString key = QString("session/editor/documents/") + QString::number(index);
    m_settings->setValue(key, editorText);
}


QList<QString> NanonSession::loadEditorContent()
{
    QList<QString> result;
    int count = 0;
    while (true) {
        QString key = QString("session/editor/documents/") + QString::number(count);
        QVariant value = m_settings->value(key);
        if (value.isNull()) {
            break;
        }
        result.push_back(value.toString());
        count ++;
    }
    return result;
}
