#include "nanon/languages/language.hpp"

#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QVariant>

#include <iostream>


using namespace nanon::languages;



NanonLanguage::NanonLanguage(QMap<QString, QVariant> languageConfig)
{
    setLanguage(languageConfig);
}


void NanonLanguage::setLanguage(QMap<QString, QVariant> languageConfig)
{
    autoClosingPairs.clear();

    if (languageConfig.contains("autoClosingPairs")) {

        QList<QVariant> rawEntries = languageConfig["autoClosingPairs"].toList();
        for (auto &rawEntry : rawEntries) {
            QMap<QString, QVariant> entry = rawEntry.toMap();

            if (entry.contains("open") && entry.contains("close")) {
                AutoClosingPair closingPair;
                closingPair.open = entry["open"].toString();
                closingPair.close = entry["close"].toString();
                if (entry.contains("notIn")) {
                    for (auto &notIn : entry["notIn"].toList()) {
                        closingPair.notInScopes.push_back(notIn.toString());
                    }
                }
                autoClosingPairs.push_back(closingPair);
            }
        }
    }
}


Edit NanonLanguage::handleKeyEvent(EditorContext &context, QKeyEvent *event)
{
    Edit edit{0, 0, "", 0};

    applyAutoClosingPairRule(context, event, edit);

    return edit;

}


void NanonLanguage::applyAutoClosingPairRule(EditorContext &context, QKeyEvent *event, Edit &edit)
{

    QString newKey = event->text();

    // Hmm
    for (auto &pair : autoClosingPairs) {
        if (newKey == pair.open) {
            std::cout << "Wow I should do something about this" << std::endl;
        }
    }

}
