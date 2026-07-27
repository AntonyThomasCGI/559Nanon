#pragma once

#include <QtCore/QString>
#include <QtCore/QVector>
#include <QtGui/QTextCursor>

namespace nanon {
namespace edits {


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


};  // namespace edits
};  // namespace nanon
