
#include "nanon/widgets/highlighter.hpp"

#include <QtGui/QTextBlock>

#include <filesystem>
#include <iostream>
#include <vector>


#ifndef RESOURCE_PATH
#define RESOURCE_PATH ""
#endif


using namespace nanon::widgets;


Highlighter::Highlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{

    std::filesystem::path resourcePath = RESOURCE_PATH;
    std::filesystem::path grammarFile = resourcePath / "syntaxes" / "MagicPython.tmLanguage.json";
    QString file = grammarFile.string().c_str();

    this->setSyntaxFromFile(file);

    m_engine = std::make_unique<textmate::TextMateEngine>(&m_grammar->root);
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

    int block = currentBlock().blockNumber();
    int prev = previousBlockState();

    BlockState state;
    if (m_stateCache.contains(block - 1)) {
        m_engine->stack = m_stateCache[block - 1].stack;
    } else {
        m_engine->stack.clear();
        m_engine->stack.push_back({&m_grammar->root, nullptr});
    }

    std::vector<textmate::Region> regions = m_engine->scanLine(text);

    state.stack = m_engine->stack;
    m_stateCache[block] = state;
    setCurrentBlockState(block);

    for (auto it = regions.rbegin(); it < regions.rend(); ++it)
    {
        textmate::Region region = *it;
        if (formats.contains(region.scope)) {
            // Set highlighting.
            setFormat(region.start, region.length, formats.value(region.scope));
        }
    }
}


QVector<QString> Highlighter::scopesAtPosition(const QTextBlock &currentBlock, int pos)
{
    int blockNum = currentBlock.blockNumber();

    if (m_stateCache.contains(blockNum - 1)) {
        m_engine->stack = m_stateCache[blockNum - 1].stack;
    } else {
        m_engine->stack.clear();
        m_engine->stack.push_back({&m_grammar->root, nullptr});
    }

    const QString text = currentBlock.text();

    // Ensure engine is in correct state for this line
    auto regions = m_engine->scanLine(text);

    QVector<QString> scopes;

    for (const auto& r : regions)
    {
        if (pos >= r.start && pos < r.start + r.length)
        {
            scopes.push_back(r.scope);
        }
    }

    return scopes;
}


void Highlighter::setSyntaxFromFile(QString fileName)
{
    nanon::io::TextMateParseError err;

    nanon::io::TextMateParser tmParser = nanon::io::TextMateParser();
    QVariant tmData = tmParser.parse(fileName, err);
    if (err.error != nanon::io::TextMateParseError::ParseError::NoError) {
        std::cout << "ERROR: " << qUtf8Printable(err.errorString) << std::endl;
        return;
    }

    QMap<QString, QVariant> map = tmData.toMap();

    m_grammar = std::make_unique<textmate::Grammar>("my scope", map);

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

    formats["string.quoted.docstring.multi.python"] = stringFormat;

    formats["constant.numeric.dec.python"] = numberFormat;

    formats["storage.type.class.python"] = keywordFormat;
}