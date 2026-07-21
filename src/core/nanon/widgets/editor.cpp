
#include "nanon/io/config.hpp"
#include "nanon/widgets/editor.hpp"

#include <QtGui/QPainter>
#include <QtGui/QTextBlock>

#include <filesystem>
#include <iostream>


#ifndef RESOURCE_PATH
#define RESOURCE_PATH ""
#endif


using namespace nanon::widgets;


NanonEditor::NanonEditor(QWidget *parent) : QPlainTextEdit(parent)
{
    this->setStyleSheet("background-color:black; color:Gainsboro");
    lineNumberArea = new LineNumberArea(this);

    m_textMateEngine = std::make_unique<textmate::TextMateEngine>();

    // TODO, make this configurable, for now always set python
    std::filesystem::path resourcePath = RESOURCE_PATH;
    std::filesystem::path grammarFile = resourcePath / "syntaxes" / "MagicPython.tmLanguage.json";
    QString file = grammarFile.string().c_str();
    m_textMateEngine->setGrammarFromFile(file);

    // Load python language config
    std::filesystem::path languagePath = resourcePath / "configs" / "python" / "language_configuration.json";
    io::ConfigParseError err;
    auto configParser = io::ConfigParser();
    QVariant confData = configParser.parse(languagePath.string().c_str(), err);
    if (err.error != io::ConfigParseError::ParseError::NoError) {
        std::cout << "ERROR Could not language config: " << qUtf8Printable(err.errorString) << std::endl;
    } else {
        std::cout << "Setting language config..." << std::endl;
        QMap<QString, QVariant> languageConfig = confData.toMap();
        m_language = std::make_unique<languages::NanonLanguage>(languageConfig);
    }


    connect(this, &NanonEditor::blockCountChanged, this, &NanonEditor::updateLineNumberAreaWidth);
    connect(this, &NanonEditor::updateRequest, this, &NanonEditor::updateLineNumberArea);
    connect(this, &NanonEditor::cursorPositionChanged, this, &NanonEditor::highlightCurrentLine);

    updateLineNumberAreaWidth(0);
    highlightCurrentLine();

    m_highlighter = std::make_unique<Highlighter>(document(), m_textMateEngine.get());
}


void NanonEditor::keyPressEvent(QKeyEvent *event)
{

    QTextCursor cursor = textCursor();
    QTextBlock block = cursor.block();
    int pos = cursor.positionInBlock();

    QVector<QString> scopes = m_textMateEngine->scopesAtPosition(block, pos);

    languages::EditorContext context{block.text(), cursor, scopes};
    languages::Edit edits = m_language->handleKeyEvent(context, event);

    if (edits.hasEdits()) {
        if (edits.removeAfterCursor) {
            cursor.setPosition(cursor.position() + edits.removeAfterCursor);
            setTextCursor(cursor);
        }
        int toDelete = edits.removeAfterCursor + edits.removeBeforeCursor;

        for (int i = 0; i < toDelete; i++) {
            cursor.deletePreviousChar();
        }

        // TODO, Convert newlines to text blocks?
        //QStringList inserts = edits.insertText.split("\n");
        //bool firstLine = true;
        //for (auto &insert : inserts) {
        //    if (!firstLine) {
        //        cursor.insertBlock();
        //    }
        //    cursor.insertText(insert);
        //    firstLine = false;
        //}
        cursor.insertText(edits.insertText);

        cursor.setPosition(cursor.position() + edits.cursorOffset);
        setTextCursor(cursor);
        return;
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