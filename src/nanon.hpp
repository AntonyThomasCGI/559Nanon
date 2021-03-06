#pragma once

#include "io/nanon_textmate.hpp"

#include <QtCore/QRegularExpression>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPlainTextEdit>
#include <QtGui/QSyntaxHighlighter>


class NanonEditor : public QPlainTextEdit
{
    Q_OBJECT

public:
    NanonEditor(QWidget *parent = nullptr);

    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();

protected:
    void resizeEvent(QResizeEvent *event) override;

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


struct Rule {
    QString name;
    QString match;
    QString begin;
    QString end;
    QString while_;
    QString include;
    QString contentName;
    std::vector<Rule> captures;  // TODO add tuple? index of captures.
    std::vector<Rule> beginCaptures;
    std::vector<Rule> endCaptures;
    std::vector<Rule> whileCaptures;
    std::vector<Rule> patterns;
};

class Highlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    Highlighter(QTextDocument *parent = 0);

protected:
    void highlightBlock(const QString &text) override;

private:
    struct HighlightingRule
    {
        QRegularExpression pattern;
        QTextCharFormat format;
    };


    Rule makeRule(QMap<QString, QVariant> map);

    QVector<HighlightingRule> highlightingRules;

    void setSyntaxFromFile(QString fileName);

    // QRegularExpression commentStartExpression;
    // QRegularExpression commentEndExpression;

    QTextCharFormat keywordFormat;
    // QTextCharFormat classFormat;
    // QTextCharFormat singleLineCommentFormat;
    // QTextCharFormat multiLineCommentFormat;
    // QTextCharFormat quotationFormat;
    // QTextCharFormat functionFormat;
};



class NanonWindow : public QMainWindow
{
    Q_OBJECT

public:
    NanonWindow(QWidget* parent = 0);
    virtual ~NanonWindow();

	void appendOutput(QString text);

protected:
    void resizeEvent(QResizeEvent *ev) override;

private:
    void createStatusBar();
    static void setMonospaced(QPlainTextEdit *textEdit);

	QPlainTextEdit *outputWindow;

    NanonEditor *editor;

    Highlighter *highlighter;

    // QHBoxLayout    *layout;
    // QPlainTextEdit *outputWindow;
    // QPlainTextEdit *editor;
};
