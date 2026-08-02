#pragma once

#include "nanon/edits/language.hpp"
#include "nanon/session.hpp"
#include "nanon/style/theme.hpp"
#include "nanon/textmate/engine.hpp"
#include "nanon/widgets/highlighter.hpp"

#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QWidget>

#include <memory>


namespace nanon {
namespace widgets {



class NanonEditor : public QPlainTextEdit
{
    Q_OBJECT

public:
    NanonEditor(NanonSession *session, QWidget *parent = nullptr);

    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();

    // TODO, idk if the main window should be accessing this.
    QVector<QString> scopesAtPosition(QTextBlock block, int pos) { return m_textMateEngine->scopesAtPosition(block, pos); };

protected:
    //void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &rect, int dy);

    void configurePalette();

private:
    QWidget *lineNumberArea;

    NanonSession *m_session;

    std::unique_ptr<textmate::TextMateEngine> m_textMateEngine;
    std::unique_ptr<edits::NanonLanguage> m_language;
    std::unique_ptr<Highlighter> m_highlighter;
};



class LineNumberArea : public QWidget
{
public:
    LineNumberArea(NanonEditor *editor) : QWidget(editor), codeEditor(editor)
    {}

    QSize sizeHint() const override
    {
        return QSize(codeEditor->lineNumberAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event) override
    {
        codeEditor->lineNumberAreaPaintEvent(event);
    }

private:
    NanonEditor *codeEditor;
};



};  // namespace widgets
};  // namespace nanon
