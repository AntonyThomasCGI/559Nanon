#pragma once

#include "io/nanon_textmate.hpp"

#include <QtCore/QRegularExpression>
#include <QtCore/QPointer>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPlainTextEdit>
#include <QtGui/QSyntaxHighlighter>
#include <QtGui/QTextDocument>


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


struct Scope {
    QString name;
    int startIndex;
    int endIndex;
};

class ScopeBlockData : public QTextBlockUserData
{
    // Q_OBJECT

public:
    QVector<Scope> scopes;
};


class Highlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    Highlighter(QTextDocument *parent = 0);

protected:
    void highlightBlock(const QString &text);

private:
    // void reformatBlocks(int from, int charsRemoved, int charsAdded);
//     // struct HighlightingRule
//     // {
//     //     QRegularExpression pattern;
//     //     QTextCharFormat format;
//     // };
    struct Rule {
        int scopeID;
        QString name;
        QRegularExpression match;
        QRegularExpression begin;
        QRegularExpression end;
        QRegularExpression while_;
        QString include;
        QString contentName;
        QMap<int, Rule> captures;
        QMap<int, Rule> beginCaptures;
        QMap<int, Rule> endCaptures;
        QMap<int, Rule> whileCaptures;
        std::vector<Rule> patterns;
    };

    // QVector<QString> previousBlockScopes;
    // QVector<QString> currentBlockScopes;

//     void setDocument(QTextDocument *doc);


    Rule makeRule(QMap<QString, QVariant> map, int &blockStateID);

//     QTextDocument *doc;

//     // QVector<HighlightingRule> highlightingRules;

    void setSyntaxFromFile(QString fileName);

//     QRegularExpression commentStartExpression;
//     QRegularExpression commentEndExpression;

    QTextCharFormat keywordFormat;
    QTextCharFormat multiLineFormat;

//     int custumBlockState;
    ScopeBlockData* previousBlockUserData() const;

    Rule rule;
//     QVector<QString> scopes;
//     // QTextCharFormat classFormat;
//     // QTextCharFormat singleLineCommentFormat;
//     // QTextCharFormat multiLineCommentFormat;
//     // QTextCharFormat quotationFormat;
//     // QTextCharFormat functionFormat;
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
