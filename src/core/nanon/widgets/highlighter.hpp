
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
    Highlighter(QTextDocument *parent, textmate::TextMateEngine *engine);

protected:
    void highlightBlock(const QString &text);

private:
    void setHackyHighlighting();
    QMap<QString, QTextCharFormat> formats;

    ScopeBlockData* previousBlockUserData() const;

    textmate::TextMateEngine *m_textMateEngine;
    style::NanonTheme *m_theme;
};


};  // namespace widgets
};  // namespace nanon
