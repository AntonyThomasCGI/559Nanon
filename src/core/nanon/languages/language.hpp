#pragma once

#include <QtCore/QRegularExpression>
#include <QtCore/QString>
#include <QtCore/QVector>
#include <QtGui/QKeyEvent>
#include <QtGui/QTextCursor>


namespace nanon {
namespace languages {


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


struct Edit
{
    /** Remove text before the cursor */
    unsigned int removeBeforeCursor = 0;

    /** Remove text after the cursor */
    unsigned int removeAfterCursor = 0;

    /** The text to insert */
    QString insertText = "";

    /** Apply an offset to the cursor after inserting text */
    int cursorOffset = 0;

    /**
     * Check if this object has any edits.
     */
    bool hasEdits() { return (insertText != "" || removeAfterCursor != 0 || removeBeforeCursor != 0 || cursorOffset != 0); };
};


struct EditorContext
{
    QString currentLine;
    QTextCursor cursor;

    QVector<QString> scopes;

    /** Get the next character occurring after the current cursor */
    QString nextCharacter() {
        int pos = cursor.positionInBlock();
        if (pos >= currentLine.length()) {
            return "\n";
        }
        return currentLine[pos];
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


};  // namespace nanon
};  // namespace languages
