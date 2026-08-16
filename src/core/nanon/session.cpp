

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


void NanonSession::saveDocument(widgets::NanonDocument *document)
{
    auto serializedDocument = document->serialize();
    QVariant setting = m_settings->value("session/editor/documents");
    QMap<QString, QVariant> documents = setting.toMap();
    documents[serializedDocument["uuid"].toString()] = serializedDocument;
    m_settings->setValue("session/editor/documents", documents);
}


bool NanonSession::deleteDocument(QUuid uuid)
{
    QVariant setting = m_settings->value("session/editor/documents");
    QMap<QString, QVariant> documents = setting.toMap();
    if (documents.contains(uuid.toString())) {
        documents.remove(uuid.toString());
        return true;
    }
    return false;
}


QList<QMap<QString, QVariant>> NanonSession::loadDocuments()
{
    QList<QMap<QString, QVariant>> result;

    QVariant setting = m_settings->value("session/editor/documents");
    QMap<QString, QVariant> rawDocs = setting.toMap();
    for (auto rawDoc : rawDocs.values()) {
        result.push_back(rawDoc.toMap());
    }

    return result;
}
