
#pragma once

#include "nanon/io/textmate.hpp"
#include "nanon/textmate/engine.hpp"
#include "nanon/textmate/grammar.hpp"
#include "nanon/widgets/editor.hpp"

#include <QtGui/QSyntaxHighlighter>
#include <QtGui/QTextDocument>
#include <QtWidgets/QWidget>

#include <memory>


namespace nanon {
namespace widgets {


class ScopeBlockData : public QTextBlockUserData
{
    // Q_OBJECT

public:
    QVector<textmate::Region> regions;
};


class Highlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    Highlighter(QTextDocument *parent = 0);

    QVector<QString> scopesAtPosition(const QTextBlock &currentBlock, int pos);

protected:
    void highlightBlock(const QString &text);

private:


    struct BlockState
    {
        std::vector<textmate::Context> stack;
    };

    void setSyntaxFromFile(QString fileName);

    QMap<QString, QTextCharFormat> formats;


    ScopeBlockData* previousBlockUserData() const;

    std::unique_ptr<textmate::Grammar> m_grammar;
    std::unique_ptr<textmate::TextMateEngine> m_engine;
    QHash<int, BlockState> m_stateCache;
};


};  // namespace widgets
};  // namespace nanon
