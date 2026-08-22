

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


QList<QString> NanonSession::getRecentActions()
{
    return m_settings->value("session/actions/recent").toStringList();
}


void NanonSession::saveRecentAction(QString actionName)
{

    QVariant recentActionsVariant = m_settings->value("session/actions/recent");
    QList<QString> recentActions = recentActionsVariant.toStringList();

    int currentIndex = recentActions.indexOf(actionName);
    if (currentIndex != -1) {
        recentActions.move(currentIndex, 0);
    } else {
        recentActions.push_front(actionName);
        if (recentActions.length() > 10) {
            recentActions.pop_back();
        }
    }

    m_settings->setValue("session/actions/recent", recentActions);
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
        int deleteCount = documents.remove(uuid.toString());
        if (deleteCount != 1) {
            qWarning() << "Failed to delete document with id " << uuid.toString();
            return false;
        }

        m_settings->setValue("session/editor/documents", documents);
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
