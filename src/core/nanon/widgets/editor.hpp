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

    void setFont(const QFont &);

    void saveEditorSession();

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

    QList<QTextDocument*> m_documents;
    int m_currentDocumentIndex;

    std::unique_ptr<textmate::TextMateEngine> m_textMateEngine;
    std::unique_ptr<edits::NanonLanguage> m_language;
    Highlighter* m_highlighter;

    void onNextDocument();
    void onPreviousDocument();
    void setDocumentIndex(int index);
    int onNewDocument();
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
