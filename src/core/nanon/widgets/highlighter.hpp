
#pragma once

#include "nanon/style/theme.hpp"
#include "nanon/textmate/engine.hpp"
#include "nanon/textmate/grammar.hpp"

#include <QtGui/QSyntaxHighlighter>
#include <QtGui/QTextDocument>
#include <QtWidgets/QWidget>

#include <memory>


namespace nanon {
namespace widgets {



class Highlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    Highlighter(QTextDocument *parent = 0);
    Highlighter(QTextDocument *parent, textmate::TextMateEngine *engine);

protected:
    void highlightBlock(const QString &text);

private:
    QMap<QString, QTextCharFormat> formats;

    textmate::TextMateEngine *m_textMateEngine;
    style::NanonTheme *m_theme;
};


};  // namespace widgets
};  // namespace nanon
