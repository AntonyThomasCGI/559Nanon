
#include "nanon.hpp"
#include "textmate/nanon_rule.hpp"

#include <QFont>
#include <QFontDatabase>
#include <QShortcut>
#include <QtCore/QRegularExpressionMatchIterator>
#include <QtCore/QTimer>
#include <QtGui/QPainter>
#include <QtGui/QTextBlock>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QMenu>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QStatusBar>

#include <filesystem>
#include <iostream>
#include <string>
#include <vector>



#ifndef RESOURCE_PATH
#define RESOURCE_PATH ""
#endif


NanonEditor::NanonEditor(QWidget *parent) : QPlainTextEdit(parent)
{
    this->setStyleSheet("background-color:black; color:Gainsboro");
    lineNumberArea = new LineNumberArea(this);

    connect(this, &NanonEditor::blockCountChanged, this, &NanonEditor::updateLineNumberAreaWidth);
    connect(this, &NanonEditor::updateRequest, this, &NanonEditor::updateLineNumberArea);
    connect(this, &NanonEditor::cursorPositionChanged, this, &NanonEditor::highlightCurrentLine);

    updateLineNumberAreaWidth(0);
    highlightCurrentLine();
}


void NanonEditor::keyPressEvent(QKeyEvent *event)
{
    // Explicitly handle Shift/Meta + Enter so that a new textBlock is inserted.
    if (event->key() == Qt::Key_Return ||
        event->key() == Qt::Key_Enter) {
            if (event->modifiers() & Qt::MetaModifier || event->modifiers() & Qt::ShiftModifier) {
                textCursor().insertBlock();
                return;
            }
    }

    QPlainTextEdit::keyPressEvent(event);
}


int NanonEditor::lineNumberAreaWidth()
{
    int digits = 1;
    int spacePadding = 0;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
        if ((digits - 1) % 3 == 0) ++spacePadding;
    }
    digits = digits + spacePadding;

    int space = 12 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * qMax(digits, 5);

    return space;
}

void NanonEditor::updateLineNumberAreaWidth(int newBlockCount)
{

    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}


void NanonEditor::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void NanonEditor::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy) {
        lineNumberArea->scroll(0, dy);
    } else {
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());
    }

    if (rect.contains(viewport()->rect())) {
        updateLineNumberAreaWidth(0);
    }
}

void NanonEditor::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;

        QColor lineColor = QColor(Qt::darkGray).darker(500);

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);
}

void NanonEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(lineNumberArea);
    QColor rectColor = QColor(Qt::black);
    painter.fillRect(event->rect(), rectColor);

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    QRectF temp = blockBoundingRect(block);
    QRectF translated = temp.translated(contentOffset());
    int top = qRound(translated.top());
    QRectF bbRect = blockBoundingRect(block);
    int bottom = top + qRound(bbRect.height());

    QFont font = this->font();

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {

            //blockBoundingRect(block).height()

            QColor darkPurple = QColor(48, 25, 52, 255);
            QPen drawPen(darkPurple, 2);
            int yValue = top + qRound(fontMetrics().height() / 2.0);
            QPoint leftPnt(10, yValue);
            QPoint rightPnt(qRound(lineNumberArea->width() / 1.5), yValue);

            painter.setPen(drawPen);
            painter.setFont(font);
            painter.drawLine(leftPnt, rightPnt);

            int width = qRound(lineNumberArea->width() / 6.0);
            QPoint upperLeftPnt(width, yValue - 6);
            QPoint upperRightPnt(width + width, yValue - 6);
            painter.drawLine(upperLeftPnt, upperRightPnt);

            if ((blockNumber + 1) % 10 == 0) {
                painter.setPen(QColor(191, 255, 0, 255));
                QString number = QString::number(blockNumber + 1);
                for (int i = number.length() - 3; i > 0; i = i - 3)
                {
                    number.insert(i, " ");
                }
                // std::cout << qUtf8Printable(modPart) << std::endl;
                painter.drawText(0, top, lineNumberArea->width() -13, fontMetrics().height(),
                             Qt::AlignRight, number);
            } else {
                painter.setPen(Qt::lightGray);
                QString number = QString::number((blockNumber + 1) % 10);
                painter.translate(lineNumberArea->width() - 13, 0);
                painter.rotate(90);
                painter.drawText(top, 0, fontMetrics().height(), fontMetrics().height(),
                                Qt::AlignCenter, number);
                painter.resetTransform();
            };
        }

        block = block.next();
        top = bottom;
        bottom = top + qRound(blockBoundingRect(block).height());
        ++blockNumber;
    }
}



NanonWindow::NanonWindow(QWidget* parent)
{
    this->setWindowTitle("559 Nanon");
    this->setStyleSheet("background-color:white;");

    QSplitter *splitter = new QSplitter(Qt::Vertical);

    outputWindow = new QPlainTextEdit;
    outputWindow->setReadOnly(true);
    outputWindow->setWordWrapMode(QTextOption::NoWrap);
    outputWindow->setStyleSheet("background-color:black; color:Gainsboro");

    editor = new NanonEditor;
    editor->setWordWrapMode(QTextOption::NoWrap);

    // Load fonts
    std::filesystem::path resourcePath = RESOURCE_PATH;
    std::filesystem::path fontPath = resourcePath / "fonts";

    std::filesystem::path defaultFont = fontPath / "Courier_Prime" / "CourierPrime-Regular.ttf";

    std::cout << "Loading font: " << defaultFont << std::endl;
    int fontId = QFontDatabase::addApplicationFont(defaultFont.string().c_str());

    QStringList fontFamilies = QFontDatabase::applicationFontFamilies(fontId);
    if (!fontFamilies.empty()) {
        std::cout << "Loaded font: " << fontFamilies.at(0).toStdString() << std::endl;
        QString fontFamily = fontFamilies.at(0);
        this->setFont(QFont(fontFamily));
        editor->setFont(QFont(fontFamily));
        outputWindow->setFont(QFont(fontFamily));
    } else {
        std::cerr << "Failed to load font: " << defaultFont.string() << std::endl;
    }

    highlighter = new Highlighter(editor->document());

    splitter->addWidget(outputWindow);
    splitter->addWidget(editor);

    setCentralWidget(splitter);

    createStatusBar();

    QShortcut *shortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_Return), this);
    connect(shortcut, &QShortcut::activated, this, &NanonWindow::onRunCode);

    QShortcut *scopesShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_S), this);
    connect(scopesShortcut, &QShortcut::activated, this, &NanonWindow::onShowScopesAtCursor);

    QString tempText = R""""(# Welcome to 559 Nanon!

import functools
from contextlib import contextmanager


class CoolClass:
    def __init__(self, x):
        self.x = x


print('this is a \'great\' test')


@contextmanager
def test_manager(resource):
    try:
        yield "Hi"
    finally:
        print('cleanup')


def fn(a, b, limit=10, count=0):
    """This is an example fibonacci function.

    Args:
        a (int): A number.
        b (int): B number.
    """
    if count >= limit:
        return

    c = a + b
    print(f"{c} (iteration={count + 1})")
    fn(b, c, limit=limit, count=count + 1)


with test_manager(None):
    pass

fn(0, 1)

i = 5
while i <= 10:
    i += 1


CONSTANT = True




)"""";

//    tempText = R""""(import functools
//from contextlib import contextmanager
//
//
//class CoolClass:
//    def __init__(self, x):
//        self.x = x
//)"""";


    //tempText = "from contextlib import ...";

    //tempText = "print('this test', why=True)";
//    tempText = R""""(import functools
//functools.partial(print, "a test \"print\" example")
//)"""";

    editor->setPlainText(tempText);

}


NanonWindow::~NanonWindow()
{}


void NanonWindow::onRunCode()
{
    if (m_interpreter == nullptr) {
        std::cout << "WARNING no interpreter set" << std::endl;
        return;
    }

    QString content = editor->toPlainText();
    std::string strContent = content.toStdString();

    ExecutionResult result = m_interpreter->executeCode(strContent);

    QString resultStdout = QString::fromStdString(result.stdout);
    QString resultStderr = QString::fromStdString(result.stderr);
    outputWindow->moveCursor(QTextCursor::End);
    outputWindow->insertPlainText(resultStdout);
}


void NanonWindow::onShowScopesAtCursor()
{
    QTextCursor cursor = editor->textCursor();
    QTextBlock currentBlock = cursor.block();
    if (!currentBlock.isValid()) {
        return;
    }

    int cursorPosition = cursor.positionInBlock();

    QVector<QString> scopes = highlighter->scopesAtPosition(currentBlock, cursorPosition);

    std::cout << std::to_string(scopes.length()) << std::endl;

    const QPoint cursorCoordinates = editor->cursorRect().bottomRight();
    QMenu menu("Scopes", this);
    bool hasScope = false;
    for (auto &scope : scopes) {
        menu.addAction(scope);
        hasScope = true;
        // std::cout << qUtf8Printable(scope.name) << " " << std::to_string(scope.startIndex) << " " << std::to_string(scope.endIndex) << std::endl;
    }
    if (!hasScope) {
        menu.addAction("Not in a scope");
    }
    menu.setStyleSheet("background-color:white;color:black;");
    menu.exec(editor->viewport()->mapToGlobal(cursorCoordinates));
}



void NanonWindow::createStatusBar()
{
    statusBar()->showMessage("Ready");
}


void NanonWindow::appendOutput(QString text)
{
    this->outputWindow->insertPlainText(text);
    this->outputWindow->moveCursor (QTextCursor::End);
}


void NanonWindow::setInterpreter(NanonInterpreterBase* interpreter)
{
    m_interpreter = interpreter;
}


Highlighter::Highlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{

    std::filesystem::path resourcePath = RESOURCE_PATH;
    std::filesystem::path grammarFile = resourcePath / "syntaxes" / "MagicPython.tmLanguage.json";
    //std::filesystem::path grammarFile = resourcePath / "syntaxes" / "TestPython4.tmLanguage.json";
    QString file = grammarFile.string().c_str();

    this->setSyntaxFromFile(file);

    m_engine = std::make_unique<TextMateEngine>(&m_grammar->root);
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

    std::vector<Region> regions = m_engine->scanLine(text);
    //std::cout << "regions for block " << std::to_string(block) << std::endl;
    //for (auto& region : regions) {
    //    std::cout << region.scope.toStdString() << " " << std::to_string(region.start) << " " << std::to_string(region.length) << std::endl;
    //}

    state.stack = m_engine->stack;
    m_stateCache[block] = state;
    setCurrentBlockState(block);

    for (auto it = regions.rbegin(); it < regions.rend(); ++it)
    {
        Region region = *it;
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
    TextMateParseError err;

    TextMateParser tmParser = TextMateParser();
    QVariant tmData = tmParser.parse(fileName, err);
    if (err.error != TextMateParseError::ParseError::NoError) {
        std::cout << "ERROR: " << qUtf8Printable(err.errorString) << std::endl;
        return;
    }

    QMap<QString, QVariant> map = tmData.toMap();

    m_grammar = std::make_unique<Grammar>("my scope", map);

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
    commentFormat.setForeground(Qt::darkGreen);
    stringFormat.setForeground(Qt::darkCyan);
    specialFormat.setForeground(QColor(191, 255, 0, 255));
    variableFormat.setForeground(Qt::lightGray);
    escapeFormat.setForeground(Qt::darkRed);
    numberFormat.setForeground(QColor(255, 0, 128, 255));

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
