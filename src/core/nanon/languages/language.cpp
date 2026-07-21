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

    if (languageConfig.contains("onEnterRules")) {
        QList<QVariant> onEnterRules = languageConfig["onEnterRules"].toList();
        for (const auto &variant : onEnterRules) {
            QMap<QString, QVariant> rawRule = variant.toMap();
            OnEnterRule onEnterRule;
            if (rawRule.contains("beforeText")) {
                onEnterRule.beforeText = rawRule["beforeText"].toString();
            }
            if (rawRule.contains("afterText")) {
                onEnterRule.afterText = rawRule["afterText"].toString();
            }
            if (rawRule.contains("action")) {
                QMap<QString, QVariant> rawAction = rawRule["action"].toMap();
                OnEnterRule::Action action;
                if (rawAction.contains("appendText")) {
                    action.appendText = rawAction["appendText"].toString();
                }
                if (rawAction.contains("indent")) {
                    QString rawIndent = rawAction["indent"].toString();
                    OnEnterRule::IndentType indentType = OnEnterRule::IndentType::NONE;
                    if (rawIndent == "indent") {
                        indentType = OnEnterRule::IndentType::INDENT;
                    } else if (rawIndent == "outdent") {
                        indentType = OnEnterRule::IndentType::OUTDENT;
                    }
                    action.indent = indentType;
                }
                onEnterRule.action = action;
            }
            m_onEnterRules.push_back(onEnterRule);
        }
    }
}


Edit NanonLanguage::handleKeyEvent(EditorContext &context, QKeyEvent *event)
{
    Edit edit;

    // TODO, Some of these actions only need to trigger on specific key events.

    applyAutoClosingPairEdits(context, event, edit);
    applySurroundingPairEdits(context, event, edit);
    applyIndentationMatchEdits(context, event, edit);
    applyTabsToSpacesEdits(context, event, edit);
    applyBackspaceIndentEdits(context, event, edit);
    applyOnEnterEdits(context, event, edit);

    return edit;
}


bool currentlyInScope(QVector<QString> currentScopes, QVector<QString> scopeList)
{
    for (const auto& currentScope : currentScopes) {
        for (const auto &scope : scopeList) {
            if (currentScope.contains(scope)) {
                return true;
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

        // If the next character already closes the pair, just move the cursor forward.
        if (newKey == pair.close && context.nextCharacter() == newKey) {
            const int openCount = context.currentLine.count(pair.open);
            const int closeCount = context.currentLine.count(pair.close);
            if (openCount == closeCount) {
                edit.cursorOffset = 1;
                return true;
            }
        }

        if (newKey == pair.open) {
            // Only apply auto closing pairs if appearing before these characters
            if (!m_autoCloseBefore.contains(context.nextCharacter())) {
                return false;
            }
            // Skip the specified not-in scopes
            if (currentlyInScope(context.scopes, pair.notInScopes)) {
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
            QString closePair = m_surroundingPairs[deleteCharacter];
            if (closePair == context.nextCharacter()) {
                const int openCount = context.currentLine.count(deleteCharacter);
                const int closeCount = context.currentLine.count(closePair);
                // If deleting the previous character balances pairs, don't remove the extra pair.
                if (openCount == closeCount + 1) {
                    return false;
                }
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
        } else {
            edit.insertText = "\n";
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


bool NanonLanguage::applyOnEnterEdits(EditorContext &context, QKeyEvent *event, Edit &edit)
{
    if (event->key() != Qt::Key_Return && event->key() != Qt::Key_Enter) {
        return false;
    }

    for (auto &rule : m_onEnterRules) {
        if (!rule.ruleApplies(context.currentLine, context.cursor.positionInBlock())) {
            continue;
        }

        // This code assumes ``applyIndentationMatchEdits`` has already ran and applied
        // new line and base indentation rules to the edit.


        QString indent = QString(m_tabWidth, ' ');
        if (rule.action.indent == OnEnterRule::IndentType::INDENT) {
            edit.insertText.append(indent);
        } else if (
            rule.action.indent == OnEnterRule::IndentType::OUTDENT
            && edit.insertText.endsWith(indent)
        ) {
            edit.insertText.chop(m_tabWidth);
        }

        if (rule.action.appendText != "") {
            edit.insertText.append(rule.action.appendText);
        }
        return true;
    }
    return false;
}