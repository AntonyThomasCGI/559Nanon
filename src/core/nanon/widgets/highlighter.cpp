
#include "nanon/io/config.hpp"
#include "nanon/widgets/highlighter.hpp"

#include <QtGui/QTextBlock>

#include <filesystem>
#include <iostream>
#include <vector>


using namespace nanon::widgets;


Highlighter::Highlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{

    setHackyHighlighting();
}

Highlighter::Highlighter(QTextDocument *parent, textmate::TextMateEngine *engine)
    : Highlighter(parent)
{
    m_textMateEngine = engine;
}


ScopeBlockData* Highlighter::previousBlockUserData() const
{
    const QTextBlock thisBlock = currentBlock();
    if (!thisBlock.isValid())
        return {};

    const QTextBlock previous = thisBlock.previous();
    ScopeBlockData *data = dynamic_cast<ScopeBlockData*> (previous.userData());

    return data;
}


void Highlighter::highlightBlock(const QString &text)
{
    if (!m_textMateEngine) {
        return;
    }

    int blockN = currentBlock().blockNumber();

    QVector<textmate::Region> regions = m_textMateEngine->parseBlock(blockN, text);

    for (auto it = regions.rbegin(); it < regions.rend(); ++it)
    {
        textmate::Region region = *it;
        for (const auto &[scope, format] : formats.asKeyValueRange()) {
            if (region.scope.startsWith(scope)) {
                // Set highlighting.
                setFormat(region.start, region.length, formats.value(scope));
            }
        }
    }
}


void Highlighter::setHackyHighlighting()
{
    QTextCharFormat keywordFormat;
    QTextCharFormat multiLineFormat;
    QTextCharFormat commentFormat;
    QTextCharFormat stringFormat;
    QTextCharFormat numberFormat;
    QTextCharFormat specialFormat;
    QTextCharFormat variableFormat;
    QTextCharFormat escapeFormat;

    keywordFormat.setForeground(Qt::darkMagenta);
    multiLineFormat.setForeground(QColor(191, 255, 0, 255));
    commentFormat.setForeground(QColor(100, 100, 100, 255));
    stringFormat.setForeground(Qt::darkCyan);
    specialFormat.setForeground(QColor(191, 255, 0, 255));
    variableFormat.setForeground(Qt::lightGray);
    escapeFormat.setForeground(Qt::darkRed);
    numberFormat.setForeground(QColor(255, 0, 128, 255));

    commentFormat.setFontItalic(true);

    // super hardcoded atm lol.
    // this will eventually read from a json file.

    formats["string.quoted.double"] = multiLineFormat;
    formats["string.quoted.single"] = multiLineFormat;
    formats["string.quoted.double.example"] = multiLineFormat;
    formats["string.quoted.single.example"] = multiLineFormat;
    formats["test.import.keyword"] = keywordFormat;
    formats["comment.line.number-sign"] = keywordFormat;
    formats["storage.modifier.async.python"] = keywordFormat;
    formats["storage.type.function.python"] = keywordFormat;
    formats["keyword.control.todo.example"] = keywordFormat;

    formats["comment.line.number-sign.python"] = commentFormat;
    formats["string.quoted.single.python"] = stringFormat;
    formats["string.quoted.double.python"] = stringFormat;
    formats["keyword.control.python"] = keywordFormat;
    formats["constant.numeric.python"] = numberFormat;


    formats["keyword.control.def.python"] = keywordFormat;
    formats["entity.name.function.python"] = variableFormat;
    formats["variable.parameter.function.python"] = variableFormat;

    formats["punctuation.section.parameters.begin.python"] = keywordFormat;
    formats["punctuation.section.parameters.end.python"] = keywordFormat;

    formats["keyword.control.import.python"] = keywordFormat;
    formats["keyword.control.flow.python"] = keywordFormat;

    formats["constant.character.escape.python"] = escapeFormat;

    formats["meta.function.decorator.python"] = specialFormat;

    formats["string.quoted.docstring"] = stringFormat;

    formats["constant.numeric.dec.python"] = numberFormat;

    formats["storage.type.class.python"] = keywordFormat;
}