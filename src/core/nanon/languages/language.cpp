#include "nanon/languages/language.hpp"

#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QVariant>

#include <algorithm>
#include <iostream>
#include <string>


using namespace nanon::languages;



NanonLanguage::NanonLanguage(QMap<QString, QVariant> languageConfig)
{
    setLanguage(languageConfig);
}


void NanonLanguage::setLanguage(QMap<QString, QVariant> languageConfig)
{
    m_autoClosingPairs.clear();
    m_surroundingPairs.clear();

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
                m_autoClosingPairs.push_back(closingPair);
            }
        }
    }

    if (languageConfig.contains("surroundingPairs")) {
        QList<QVariant> rawEntries = languageConfig["surroundingPairs"].toList();
        for (auto &rawEntry : rawEntries) {
            QList<QVariant> pair = rawEntry.toList();
            QString first = pair[0].toString();
            QString second = pair[1].toString();

            m_surroundingPairs[first] = second;
        }
    }
}


Edit NanonLanguage::handleKeyEvent(EditorContext &context, QKeyEvent *event)
{
    Edit edit{0, 0, "", 0};

    // TODO, Some of these actions only need to trigger on specific key events.

    if (applyAutoClosingPairEdits(context, event, edit)) { return edit; };
    if (applySurroundingPairEdits(context, event, edit)) { return edit; };
    if (applyIndentationMatchEdits(context, event, edit)) { return edit; };
    if (applyTabsToSpacesEdits(context, event, edit)) { return edit; };
    if (applyBackspaceIndentEdits(context, event, edit)) { return edit; };

    return edit;
}


bool currentScopeIncludesScope(QVector<QString> currentScopes, QVector<QString> scopeList)
{
    for (const auto& currentScope : currentScopes) {
        for (const auto &scope : scopeList) {
            if (currentScope.contains(scope)) {
                return false;
            }
        }
    }
    return false;
}


bool NanonLanguage::applyAutoClosingPairEdits(EditorContext &context, QKeyEvent *event, Edit &edit)
{
    QString newKey = event->text();

    for (auto &pair : m_autoClosingPairs) {
        if (newKey != pair.open && newKey != pair.close) {
            continue;
        }

        const int pos = context.cursor.positionInBlock();
        const bool hasNextChar = pos < context.currentLine.length();
        const bool nextCharacterSame = hasNextChar && context.currentLine[pos] == newKey;

        // If the next character already closes the pair, just move the cursor forward.
        if (newKey == pair.close && nextCharacterSame) {
            const int openCount = context.currentLine.count(pair.open);
            const int closeCount = context.currentLine.count(pair.close);
            if (openCount == closeCount) {
                edit.cursorOffset = 1;
                return true;
            }
        }

        if (newKey == pair.open) {
            if (currentScopeIncludesScope(context.scopes, pair.notInScopes)) {
                return false;
            }
            edit.insertText = newKey + pair.close;
            edit.cursorOffset = -1;
            return true;
        }

        return false;
    }
    return false;
}


bool NanonLanguage::applySurroundingPairEdits(EditorContext &context, QKeyEvent *event, Edit &edit)
{
    if (event->key() == Qt::Key_Backspace && !context.cursor.hasSelection()) {
        int pos = context.cursor.positionInBlock();
        int lineLength = context.currentLine.length();
        if (pos == 0 || pos == lineLength) {
            return false;
        }

        QString deleteCharacter = context.currentLine[pos - 1];
        if (m_surroundingPairs.contains(deleteCharacter)) {
            QString nextCharacter = context.currentLine[pos];
            if (m_surroundingPairs[deleteCharacter] == nextCharacter) {
                edit.removeBeforeCursor = 1;
                edit.removeAfterCursor = 1;
                return true;
            }
        }
    }
    return false;
}


bool NanonLanguage::applyIndentationMatchEdits(EditorContext &context,  QKeyEvent *event, Edit &edit)
{
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        auto match = m_indentationRegex.match(context.currentLine);
        if (match.hasMatch()) {
            QString indentation = match.captured(1);
            edit.insertText = "\n" + indentation;
            return true;
        }
    }
    return false;
}


bool NanonLanguage::applyTabsToSpacesEdits(EditorContext &context, QKeyEvent *event, Edit &edit)
{
    if (event->key() == Qt::Key_Tab) {
        edit.insertText = QString(m_tabWidth, ' ');
        return true;
    }
    return false;
}


bool NanonLanguage::applyBackspaceIndentEdits(EditorContext &context, QKeyEvent *event, Edit &edit)
{
    if (event->key() == Qt::Key_Backspace) {
        auto match = m_indentationRegex.match(context.currentLine);
        if (match.hasMatch() && match.capturedLength(0) == context.currentLine.length()) {
            int lineLength = static_cast<int>(match.capturedLength(0));
            edit.removeBeforeCursor = std::min(m_tabWidth, lineLength);
            return true;
        }
    }
    return false;
}
