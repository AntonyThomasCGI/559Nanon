#pragma once

#include "edit.hpp"

#include <QtCore/QRegularExpression>
#include <QtCore/QString>
#include <QtCore/QVector>
#include <QtGui/QKeyEvent>



namespace nanon {
namespace edits {


struct AutoClosingPair
{
    QString open;
    QString close;
    QVector<QString> notInScopes;
};


struct OnEnterRule
{
    enum IndentType {NONE, INDENT, OUTDENT};

    struct Action {
        IndentType indent;
        QString appendText;
    };

    Action action;
    QString beforeText;
    QString afterText;

    bool ruleApplies(QString line, int pos) {
        if (beforeText != nullptr) {
            QString beforeLine = line.sliced(0, pos);
            QRegularExpression regex(beforeText);
            auto match = regex.match(beforeLine);
            if (!match.hasMatch()) {
                return false;
            }
        }

        if (afterText != nullptr) {
            QString afterLine = line.sliced(pos, -1);
            QRegularExpression regex(afterText);
            auto match = regex.match(afterLine);
            if (!match.hasMatch()) {
                return false;
            }
        }

        return true;
    }
};


class NanonLanguage
{
public:
    NanonLanguage() = default;
    NanonLanguage(QMap<QString, QVariant>);
    virtual ~NanonLanguage() = default;

    void setLanguage(QMap<QString, QVariant> languageConfig);

    Edit handleKeyEvent(EditorContext &context, QKeyEvent *event);

private:
    bool applyAutoClosingPairEdits(EditorContext &context, QKeyEvent *event, Edit &edit);
    bool applySurroundingPairEdits(EditorContext &context, QKeyEvent *event, Edit &edit);
    bool applyIndentationMatchEdits(EditorContext &context, QKeyEvent *event, Edit &edit);
    bool applyTabsToSpacesEdits(EditorContext &context, QKeyEvent *event, Edit &edit);
    bool applyBackspaceIndentEdits(EditorContext &context, QKeyEvent *event, Edit &edit);
    bool applyOnEnterEdits(EditorContext &context, QKeyEvent *event, Edit &edit);

    QRegularExpression m_indentationRegex = QRegularExpression("^( +|\t+)");

    QString m_autoCloseBefore = " })]\"\n";

    int m_tabWidth = 4;

    QVector<AutoClosingPair> m_autoClosingPairs;
    QHash<QString, QString> m_surroundingPairs;
    QVector<OnEnterRule> m_onEnterRules;
};


};  // namespace edits
};  // namespace nanon
