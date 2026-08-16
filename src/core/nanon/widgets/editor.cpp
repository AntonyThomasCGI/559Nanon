
#include "nanon/commands/action.hpp"
#include "nanon/io/config.hpp"
#include "nanon/style/theme.hpp"
#include "nanon/style/theme_manager.hpp"
#include "nanon/widgets/editor.hpp"

#include <QStyle>
#include <QtGui/QPainter>
#include <QtGui/QTextBlock>

#include <filesystem>
#include <iostream>
#include <math.h>


#ifndef RESOURCE_PATH
#define RESOURCE_PATH ""
#endif


using namespace nanon::widgets;


NanonEditor::NanonEditor(QSharedPointer<NanonSession> session, QWidget *parent)
    : QPlainTextEdit(parent), m_session(session)
{
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
        m_language = std::make_unique<edits::NanonLanguage>(languageConfig);
    }

    auto savedDocuments = m_session->loadDocuments();
    for (auto rawDoc : savedDocuments) {
        auto *doc = new NanonDocument(rawDoc, this);
        doc->setDocumentLayout(new QPlainTextDocumentLayout(doc));
        m_documents.push_back(doc);
    }
    if (m_documents.isEmpty()) {
        // Create default empty document
        onNewDocument();
    }

    // TODO, save the active document index. For now just set last index.
    m_currentDocumentIndex = m_documents.length() - 1;
    setDocument(m_documents.at(m_currentDocumentIndex));

    m_highlighter = new Highlighter(document(), m_textMateEngine.get());

    configurePalette();

    connect(m_session.get(), &NanonSession::themeChanged, this, &NanonEditor::configurePalette);

    connect(this, &NanonEditor::blockCountChanged, this, &NanonEditor::updateLineNumberAreaWidth);
    connect(this, &NanonEditor::updateRequest, this, &NanonEditor::updateLineNumberArea);
    connect(this, &NanonEditor::cursorPositionChanged, this, &NanonEditor::highlightCurrentLine);

    updateLineNumberAreaWidth(0);
    highlightCurrentLine();

    auto nextDocumentAction = new commands::NanonAction("Editor: Next Document", QKeySequence(Qt::META | Qt::Key_Tab), this);
    connect(nextDocumentAction, &commands::NanonAction::triggered, this, &NanonEditor::onNextDocument);

    auto previousDocumentAction = new commands::NanonAction("Editor: Previous Document", QKeySequence(Qt::META | Qt::SHIFT | Qt::Key_Tab), this);
    connect(previousDocumentAction, &commands::NanonAction::triggered, this, &NanonEditor::onPreviousDocument);

    auto newDocumentAction = new commands::NanonAction("Editor: New Document", QKeySequence(Qt::META | Qt::Key_N), this);
    connect(newDocumentAction, &commands::NanonAction::triggered, this, &NanonEditor::onNewDocument);

    auto deleteDocumentAction = new commands::NanonAction("Editor: Delete Document", this);
    connect(deleteDocumentAction, &commands::NanonAction::triggered, this, &NanonEditor::onDeleteDocument);
}


NanonEditor::~NanonEditor()
{
    saveEditorSession();
}


void NanonEditor::setDocument(NanonDocument *document)
{
    QPlainTextEdit::setDocument(document);

    int cursorPos = document->cursorPosition();

    if (cursorPos != -1) {
        auto cur = textCursor();
        cur.setPosition(cursorPos);
        setTextCursor(cur);
    }
}


void NanonEditor::configurePalette()
{
    auto theme = m_session->currentTheme();
    if (theme == nullptr) {
        std::cout << "WARNING no theme set" << std::endl;
        return;
    }
    QPalette palette;

    palette.setColor(
        QPalette::Highlight,
        QColor(theme->getColor("editorSelection.background"))
    );
    palette.setColor(
        QPalette::HighlightedText,
        QColor(theme->getColor("editor.foreground"))
    );
    palette.setColor(
        QPalette::Text,
        QColor(theme->getColor("editor.foreground"))
    );
    palette.setColor(
        QPalette::Base,
        QColor(theme->getColor("editor.background"))
    );

    setPalette(palette);

    // Refresh to get new palette colors
    m_highlighter->rehighlight();
    highlightCurrentLine();

    style()->unpolish(this);
    style()->polish(this);
    update();
    this->viewport()->setPalette(this->palette());

    viewport()->update();
    viewport()->updateGeometry();
}


void NanonEditor::setFont(const QFont &font)
{
    QPlainTextEdit::setFont(font);
    for (auto document : m_documents) {
        document->setDefaultFont(font);
    }
}


void NanonEditor::keyPressEvent(QKeyEvent *event)
{

    QTextCursor cursor = textCursor();
    QTextBlock block = cursor.block();
    int pos = cursor.positionInBlock();

    QVector<QString> scopes = m_textMateEngine->scopesAtPosition(block, pos);

    edits::EditorContext context{block.text(), cursor, scopes};
    edits::Edit edit = m_language->handleKeyEvent(context, event);

    if (edit.hasEdits()) {
        cursor.beginEditBlock();

        if (edit.removeAfterCursor) {
            cursor.setPosition(cursor.position() + edit.removeAfterCursor);
            setTextCursor(cursor);
        }
        int toDelete = edit.removeAfterCursor + edit.removeBeforeCursor;

        for (int i = 0; i < toDelete; i++) {
            cursor.deletePreviousChar();
        }

        // TODO, Convert newlines to text blocks?
        //QStringList inserts = edit.insertText.split("\n");
        //bool firstLine = true;
        //for (auto &insert : inserts) {
        //    if (!firstLine) {
        //        cursor.insertBlock();
        //    }
        //    cursor.insertText(insert);
        //    firstLine = false;
        //}
        cursor.insertText(edit.insertText);

        cursor.setPosition(cursor.position() + edit.cursorOffset);
        setTextCursor(cursor);

        cursor.endEditBlock();
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

    int space = 16 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * qMax(digits, 5);

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
    if (textCursor().hasSelection() || isReadOnly()) {
        setExtraSelections(QList<QTextEdit::ExtraSelection>{});
        return;
    }

    QList<QTextEdit::ExtraSelection> extraSelections;

    QTextEdit::ExtraSelection selection;

    auto theme = m_session->currentTheme();

    QColor lineColor = QColor(theme->getColor("editor.lineHighlightBackground"));
    selection.format.setBackground(lineColor);
    selection.format.setProperty(QTextFormat::FullWidthSelection, true);
    selection.cursor = textCursor();
    selection.cursor.clearSelection();
    extraSelections.append(selection);

    setExtraSelections(extraSelections);
}

void NanonEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    auto theme = m_session->currentTheme();

    QPainter painter(lineNumberArea);
    QColor bgColor = theme->getColor("editor.background");
    QColor lineNumberColor = theme->getColor("editorLineNumber.foreground");
    QColor lineNumberSpecialColor = theme->getColor("editorLineNumberSpecial.foreground");
    QColor rulerColor = theme->getColor("editorRuler.foreground");

    painter.fillRect(event->rect(), bgColor);

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

            QPen drawPen(rulerColor, 2);
            int yValue = top + qRound(fontMetrics().height() / 2.0);
            QPoint leftPnt(5, yValue);
            QPoint rightPnt(qRound(lineNumberArea->width() / 3.0), yValue);

            painter.setPen(drawPen);
            painter.setFont(font);
            painter.drawLine(leftPnt, rightPnt);

            int width = qRound(lineNumberArea->width() / 7.0);
            QPoint upperLeftPnt(5, yValue - 7);
            QPoint upperRightPnt(5 + width, yValue - 7);
            painter.drawLine(upperLeftPnt, upperRightPnt);

            if ((blockNumber + 1) % 10 == 0) {
                painter.setPen(lineNumberSpecialColor);
                QString number = QString::number(blockNumber + 1);
                for (int i = number.length() - 3; i > 0; i = i - 3)
                {
                    number.insert(i, " ");
                }
                painter.drawText(0, top, lineNumberArea->width() - 12, fontMetrics().height(),
                             Qt::AlignRight, number);
            } else {
                painter.setPen(lineNumberColor);
                QString number = QString::number((blockNumber + 1) % 10);
                painter.translate(lineNumberArea->width() - 12, 0);
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


void NanonEditor::onNextDocument()
{
    setDocumentIndex(m_currentDocumentIndex + 1);
}

void NanonEditor::onPreviousDocument()
{
    setDocumentIndex(m_currentDocumentIndex - 1);
}


void NanonEditor::setDocumentIndex(int index)
{

    if (index < 0 || index >= m_documents.length()) {
        return;
    }

    if (index == m_currentDocumentIndex) {
        return;
    }

    // Save the current document and cursor postion
    saveEditorSession();

    auto doc = m_documents.at(index);
    setDocument(doc);

    if (m_highlighter != nullptr) {
        m_highlighter->setDocument(doc);
    }

    m_currentDocumentIndex = index;

    emit tabChanged(index);
}


int NanonEditor::onNewDocument()
{
    std::cout << "OnNewDocument" << std::endl;
    NanonDocument *doc = new NanonDocument(this);
    doc->setDocumentLayout(new QPlainTextDocumentLayout(doc));
    doc->setDefaultFont(font());
    m_documents.push_back(doc);

    int newIdx = m_documents.length() - 1;
    setDocumentIndex(newIdx);

    emit tabCountChanged(m_documents.length());
    return newIdx;
}


void NanonEditor::onDeleteDocument()
{
    auto *doc = m_documents.at(m_currentDocumentIndex);
    m_session->deleteDocument(doc->uuid());

    m_documents.remove(m_currentDocumentIndex);


    if (m_documents.isEmpty()) {
        m_currentDocumentIndex = onNewDocument();
        return;
    }

    int newIndex = qMax(0, m_currentDocumentIndex - 1);
    setDocumentIndex(newIndex);

    emit tabCountChanged(m_documents.length());
}


void NanonEditor::saveEditorSession()
{
    auto doc = static_cast<NanonDocument*>(document());
    if (doc == nullptr) {
        return;
    }

    doc->setCursorPosition(textCursor().position());

    m_session->saveDocument(doc);
}