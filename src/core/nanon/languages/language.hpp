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


struct Edit
{
    /** Remove text before the cursor */
    unsigned int removeBeforeCursor;

    /** Remove text after the cursor */
    unsigned int removeAfterCursor;

    /** The text to insert */
    QString insertText;

    /** Apply an offset to the cursor after inserting text */
    int cursorOffset;

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

    QRegularExpression m_indentationRegex = QRegularExpression("^( +|\t+)");

    // TODO unused + \n
    QString m_autoCloseBefore = " })]";

    int m_tabWidth = 4;

    QVector<AutoClosingPair> m_autoClosingPairs;
    QHash<QString, QString> m_surroundingPairs;
};


};  // namespace nanon
};  // namespace languages
