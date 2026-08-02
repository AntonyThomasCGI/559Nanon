
#include "nanon/io/config.hpp"
#include "nanon/style/theme_manager.hpp"
#include "nanon/widgets/highlighter.hpp"

#include <QtGui/QTextBlock>

#include <filesystem>
#include <iostream>
#include <vector>


using namespace nanon::widgets;


Highlighter::Highlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
}

Highlighter::Highlighter(QTextDocument *parent, textmate::TextMateEngine *engine)
    : Highlighter(parent)
{
    m_textMateEngine = engine;
}


void Highlighter::highlightBlock(const QString &text)
{
    if (!m_textMateEngine) {
        return;
    }

    int blockN = currentBlock().blockNumber();

    QVector<textmate::Region> regions = m_textMateEngine->parseBlock(blockN, text);

    style::NanonTheme* theme = style::NanonThemeManager::instance().theme();

    if (theme != nullptr) {
        for (auto it = regions.rbegin(); it < regions.rend(); ++it) {
            textmate::Region region = *it;
            for (const auto &tokenColor : theme->tokenColors()) {
                for (const auto &tokenScope : tokenColor.scopes) {
                    if (region.scope.startsWith(tokenScope)) {
                        //std::cout << "setting format scope: " << std::to_string(region.start) << ", " <<
                        //    std::to_string(region.length) << ", " << tokenColor.format.foreground().color().name().toStdString() << ", format key: " <<
                        //    tokenScope.toStdString() << std::endl;
                        setFormat(region.start, region.length, tokenColor.format);
                        break;
                    }
                }
            }
        }
    }
}
