
#include "nanon.hpp"

#include <iostream>

#include <QtWidgets/QStatusBar>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QSplitter>
#include <QPainter.h>
#include <QTextBlock>



NanonEditor::NanonEditor(QWidget *parent) : QPlainTextEdit(parent)
{
    this->setStyleSheet("background-color:black");
    lineNumberArea = new LineNumberArea(this);

    connect(this, &NanonEditor::blockCountChanged, this, &NanonEditor::updateLineNumberAreaWidth);
    connect(this, &NanonEditor::updateRequest, this, &NanonEditor::updateLineNumberArea);
    connect(this, &NanonEditor::cursorPositionChanged, this, &NanonEditor::highlightCurrentLine);

    updateLineNumberAreaWidth(0);
    highlightCurrentLine();
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

void NanonEditor::updateLineNumberAreaWidth(int /* newBlockCount */)
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
    if (dy)
        lineNumberArea->scroll(0, dy);
    else
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
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
    // QColor rectColor = this->palette().color(QPalette::Window);
    painter.fillRect(event->rect(), rectColor);

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    QRectF temp = blockBoundingRect(block);
    QRectF translated = temp.translated(contentOffset());
    int top = qRound(translated.top());
    QRectF bbRect = blockBoundingRect(block);
    int bottom = top + qRound(bbRect.height());

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QColor darkPurple = QColor(48, 25, 52, 255);
            QPen drawPen(darkPurple, 2);
            int yValue = top + qRound(fontMetrics().height() / 2.0);
            QPoint leftPnt(0, yValue);
            QPoint rightPnt(qRound(lineNumberArea->width() / 1.5), yValue);

            painter.setPen(drawPen);
            painter.drawLine(leftPnt, rightPnt);

            int width = qRound(lineNumberArea->width() / 6.0);
            QPoint upperLeftPnt(width, yValue - 2);
            QPoint upperRightPnt(width + width, yValue - 2);
            painter.drawLine(upperLeftPnt, upperRightPnt);

            // QTextOption textOption = QTextOption(Qt::Alignment::)
            // painter.save();

            if ((blockNumber + 1) % 10 == 0 || blockNumber == 0) {
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
            // painter.restore();
        }

        block = block.next();
        top = bottom;
        bottom = top + qRound(blockBoundingRect(block).height());
        ++blockNumber;
    }
}




NanonWindow::NanonWindow(QWidget* parent)
{
    this->setStyleSheet("background-color:white;");

    QSplitter *splitter = new QSplitter(Qt::Vertical);

    // QVBoxLayout *layout = new QVBoxLayout;
    // layout->setSpacing(0);

    outputWindow = new QPlainTextEdit;
    outputWindow->setReadOnly(true);
    outputWindow->setWordWrapMode(QTextOption::NoWrap);
    setMonospaced(outputWindow);
    outputWindow->setStyleSheet("background-color:black;");
    // QTextDocument *doc = outputWindow->document();
    // QFont font("monospaced");
    // font.setStyleHint(QFont::Monospace);
    // doc->setDefaultFont(font);

    editor = new NanonEditor;
    editor->setWordWrapMode(QTextOption::NoWrap);
    setMonospaced(editor);
    // QTextDocument *editorDoc = editor->document();
    // QFont editorFont("monospaced");
    // editorFont.setStyleHint(QFont::Monospace);
    // editorDoc->setDefaultFont(editorFont);

    splitter->addWidget(outputWindow);
    splitter->addWidget(editor);

    setCentralWidget(splitter);
    // centralWidget()->setLayout(layout);

    createStatusBar();

    setSyntaxFromFile("C:\\dev\\559Nanon\\pip-requirements.tmLanguage.json");
}

NanonWindow::~NanonWindow()
{}

void NanonWindow::createStatusBar()
{
    statusBar()->showMessage("Ready");
}


void NanonWindow::appendOutput(QString text)
{
    this->outputWindow->insertPlainText(text);
    this->outputWindow->moveCursor (QTextCursor::End);
}

void NanonWindow::setMonospaced(QPlainTextEdit *textEdit)
{
    QTextDocument *doc = textEdit->document();
    QFont font("monospaced");
    font.setStyleHint(QFont::Monospace);
    // QFont font = doc->defaultFont();
    // font.setFamily("Courier New");
    doc->setDefaultFont(font);
}

void NanonWindow::resizeEvent(QResizeEvent *ev)
{
    // hacky work around for monospace being lost on window change.
    setMonospaced(outputWindow);
    setMonospaced(editor);
}

void NanonWindow::setSyntaxFromFile(QString fileName)
{
    std::optional<QJsonDocument> doc = parseJsonFile(fileName);
    if (!doc) {
        // TODO: read default file?
        // TODO: edit status bar?
        return;
    };

    QMap<QString, QVariant> map = doc.value().toVariant().toMap();
    std::cout << qUtf8Printable(map["scopeName"].toString()) << std::endl;
}
