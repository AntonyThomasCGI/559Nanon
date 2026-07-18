#pragma once

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
    int removeBeforeCursor;
    int removeAfterCursor;
    QString insertText;
    int finalCursorOffset;
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
    void applyAutoClosingPairRule(EditorContext &context, QKeyEvent *event, Edit &edit);
    QVector<AutoClosingPair> autoClosingPairs;
};


};  // namespace nanon
};  // namespace languages
