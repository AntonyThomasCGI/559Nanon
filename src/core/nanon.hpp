#pragma once

#include "io/nanon_textmate.hpp"
#include "textmate/nanon_engine.hpp"
#include "textmate/nanon_grammar.hpp"
#include "interpreters/nanon_interpreter.hpp"

#include <QtCore/QRegularExpression>
#include <QtCore/QPointer>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPlainTextEdit>
#include <QtGui/QSyntaxHighlighter>
#include <QtGui/QTextDocument>

#include <memory>


class NanonEditor : public QPlainTextEdit
{
    Q_OBJECT

public:
    NanonEditor(QWidget *parent = nullptr);

    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();

protected:
    void resizeEvent(QResizeEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &rect, int dy);

private:
    QWidget *lineNumberArea;
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


class ScopeBlockData : public QTextBlockUserData
{
    // Q_OBJECT

public:
    QVector<Region> regions;
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
        std::vector<Context> stack;
    };

    void setSyntaxFromFile(QString fileName);

    QMap<QString, QTextCharFormat> formats;


    ScopeBlockData* previousBlockUserData() const;

    std::unique_ptr<Grammar> m_grammar;
    std::unique_ptr<TextMateEngine> m_engine;
    QHash<int, BlockState> m_stateCache;
};


class NanonWindow : public QMainWindow
{
    Q_OBJECT

public:
    NanonWindow(QWidget* parent = 0);
    virtual ~NanonWindow();

	void appendOutput(QString text);

    void setInterpreter(NanonInterpreterBase* interpreter);

private:
    void createStatusBar();
    //static void setMonospaced(QPlainTextEdit *textEdit);

	QPlainTextEdit *outputWindow;

    NanonEditor *editor;

    Highlighter *highlighter;

    NanonInterpreterBase* m_interpreter = nullptr;

    void onRunCode();
    void onShowScopesAtCursor();

    // QHBoxLayout    *layout;
    // QPlainTextEdit *outputWindow;
    // QPlainTextEdit *editor;
};
