
#include "nanon.hpp"

#include <iostream>
#include <vector>
#include <string>

#include <QtCore/QRegularExpressionMatchIterator>
#include <QtWidgets/QMenu>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QSplitter>
#include <QtGui/QPainter>
#include <QtGui/QTextBlock>
#include <QtCore/QTimer>


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

void NanonEditor::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == 83 && e->modifiers() == (Qt::KeyboardModifier::AltModifier | Qt::KeyboardModifier::ShiftModifier)) {
        QTextCursor cursor = textCursor();
        QTextBlock currentBlock = cursor.block();
        if (!currentBlock.isValid())
            return;

        int cursorPosition = cursor.positionInBlock();

        QTextBlockUserData *userData = currentBlock.userData();
        ScopeBlockData *scopeData = dynamic_cast<ScopeBlockData*> (userData);
        if (scopeData!=NULL) {
            const QPoint cursorCoordinates = cursorRect().bottomRight();
            QMenu menu("Scopes", this);
            bool hasScope = false;
            for (auto & scope : scopeData->scopes) {
                if (scope.startIndex <= cursorPosition && (cursorPosition <= scope.endIndex || scope.endIndex == -1)) {
                    menu.addAction(scope.name);
                    hasScope = true;
                    // std::cout << qUtf8Printable(scope.name) << " " << std::to_string(scope.startIndex) << " " << std::to_string(scope.endIndex) << std::endl;
                }
            }
            if (!hasScope) {
                menu.addAction("Not in a scope");
            }
            menu.exec(mapToGlobal(cursorCoordinates));
        }
    } else {
        QPlainTextEdit::keyPressEvent(e);
    }
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

    outputWindow = new QPlainTextEdit;
    outputWindow->setReadOnly(true);
    outputWindow->setWordWrapMode(QTextOption::NoWrap);
    setMonospaced(outputWindow);
    outputWindow->setStyleSheet("background-color:black;");

    editor = new NanonEditor;
    editor->setWordWrapMode(QTextOption::NoWrap);
    setMonospaced(editor);

    highlighter = new Highlighter(editor->document());

    splitter->addWidget(outputWindow);
    splitter->addWidget(editor);

    setCentralWidget(splitter);
    // centralWidget()->setLayout(layout);

    createStatusBar();

    QString tempText = R""""(
import hi

def hi():
	pass

print('hi')

#comment . asd
print('next') # comment
'
wow this did not work

lmao
time to unexist myself
' hi '
more' done

'the quick brown fox jumps over the lazy dog'

"the quick brown fox jumps over the lazy dog"

"hi"

async def my_func():
	pass





)"""";
    editor->setPlainText(tempText);
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


Highlighter::Highlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    // QString file = "C:\\dev\\559Nanon\\pip-requirements.tmLanguage.json";
    // QString file = "C:\\dev\\559Nanon\\Python.tmLanguage";
    QString file = "C:\\dev\\559Nanon\\test.tmLanguage.json";

    this->setSyntaxFromFile(file);
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
    ScopeBlockData *scopeData = new ScopeBlockData;

    for (Rule & pattern : rule.patterns) {
        if (pattern.match != QRegularExpression{}) {
            QRegularExpressionMatchIterator matchIterator = pattern.match.globalMatch(text);

            while (matchIterator.hasNext()) {
                QRegularExpressionMatch match = matchIterator.next();
                setFormat(match.capturedStart(), match.capturedLength(), keywordFormat);

                if (!pattern.captures.isEmpty()) {
                    // Try to match captures.
                    // TODO: This is hardcoded to expect name as the only thing in Rule.
                    QMapIterator<int, Rule> it(pattern.captures);
                    while (it.hasNext()) {
                        it.next();
                        int key = it.key();
                        Rule value = it.value();
                        std::cout << qUtf8Printable(value.name) << std::endl;

                        QString capGroup = match.captured(key);
                        if (capGroup.isNull())
                            continue;

                        int start = text.indexOf(capGroup);
                        Scope thisScope = {value.name, start, start + capGroup.length()};
                        scopeData->scopes.push_back(thisScope);
                    }
                } else {
                    Scope thisScope = {pattern.name, match.capturedStart(), match.capturedEnd()};
                    scopeData->scopes.push_back(thisScope);
                }
            }
        }
    }

    ScopeBlockData *prevScopeData = previousBlockUserData();

    QVector<Scope> existingScopes;
    if (prevScopeData != NULL) {
        for (Scope prevScope : prevScopeData->scopes) {
            if (prevScope.endIndex == -1) {
                // Scope did not end.
                existingScopes.push_back(prevScope);
            }
        }
    }

    for (Rule & pattern : rule.patterns) {
        if (pattern.begin != QRegularExpression{} && pattern.end != QRegularExpression{} && pattern.name != QString{}) {
            int startIndex = 0;

            for (Scope scope : existingScopes) {
                if (scope.name == pattern.name) {
                    // We are already in scope.
                    QRegularExpressionMatch match = pattern.end.match(text, startIndex);
                    int endIndex = match.capturedStart();
                    if (endIndex > -1) {
                        // End of capture found, remove from next scopes.
                        setFormat(startIndex, endIndex + 1, multiLineFormat);

                        // Add the new scope.
                        Scope newScope = {pattern.name, -1, endIndex};
                        scopeData->scopes.push_back(newScope);

                        startIndex = endIndex + 1; // Don't match the same symbol.
                    } else {
                        // Not found, set whole line.
                        setFormat(startIndex, text.length(), multiLineFormat);
                        startIndex = text.length();

                        // Add scope to scope data.
                        Scope newScope = {pattern.name, -1, -1};
                        scopeData->scopes.push_back(newScope);
                    }
                    break;
                }
            }

            startIndex = text.indexOf(pattern.begin, startIndex);
            while (startIndex >= 0) {
                QRegularExpressionMatch match = pattern.end.match(text, startIndex + 1);
                int endIndex = match.capturedStart();
                int capturedLength = 0;
                if (endIndex == -1) {  // End is not in this block, add to scopes for next block.
                    capturedLength = text.length() - startIndex;

                } else {  // End of capture found, remove from scopes.
                    capturedLength = endIndex - startIndex + match.capturedLength();
                }
                Scope newScope = {pattern.name, startIndex, endIndex};
                scopeData->scopes.push_back(newScope);

                setFormat(startIndex, capturedLength, multiLineFormat);
                startIndex = text.indexOf(pattern.begin, startIndex + capturedLength);
            }
        }
    }
    setCurrentBlockUserData(scopeData);
}


Highlighter::Rule Highlighter::makeRule(QMap<QString, QVariant> map, int &blockStateID)
{
    Rule rule = Rule();

    rule.scopeID = blockStateID;
    blockStateID++;
    if (map.contains("name"))
        rule.name = map.value("name").toString();
    if (map.contains("match"))
        rule.match = QRegularExpression(map.value("match").toString());
    if (map.contains("begin"))
        rule.begin = QRegularExpression(map.value("begin").toString());
    if (map.contains("end"))
        rule.end = QRegularExpression(map.value("end").toString());
    if (map.contains("while"))
        rule.while_ = QRegularExpression(map.value("while").toString());
    if (map.contains("include"))
        rule.include = map.value("include").toString();
    if (map.contains("contentName"))
        rule.contentName = map.value("contentName").toString();

    if (map.contains("patterns")) {
        QList<QVariant> allPatterns = map.value("patterns").toList();
        for (int i = 0; i < allPatterns.size(); ++i) {
            QMap<QString, QVariant> patternMap = allPatterns.at(i).toMap();
            rule.patterns.push_back(makeRule(patternMap, blockStateID));
        }
    }

    if (map.contains("captures")) {
        QMap<QString, QVariant> capturesMap = map.value("captures").toMap();
        QMapIterator<QString, QVariant> it(capturesMap);
        while (it.hasNext()) {
            it.next();
            QMap<QString, QVariant> m = it.value().toMap();
            rule.captures.insert(it.key().toInt(), makeRule(m, blockStateID));
        }
    }

    if (map.contains("beginCaptures")) {
        QMap<QString, QVariant> beginCapturesMap = map.value("beginCaptures").toMap();
        QMapIterator<QString, QVariant> it(beginCapturesMap);
        while (it.hasNext()) {
            it.next();
            QMap<QString, QVariant> m = it.value().toMap();
            rule.beginCaptures.insert(it.key().toInt(), makeRule(m, blockStateID));
        }
    }

    if (map.contains("endCaptures")) {
        QMap<QString, QVariant> endCapturesMap = map.value("endCaptures").toMap();
        QMapIterator<QString, QVariant> it(endCapturesMap);
        while (it.hasNext()) {
            it.next();
            QMap<QString, QVariant> m = it.value().toMap();
            rule.endCaptures.insert(it.key().toInt(), makeRule(m, blockStateID));
        }
    }

    if (map.contains("whileCaptures")) {
        QMap<QString, QVariant> whileCapturesMap = map.value("whileCaptures").toMap();
        QMapIterator<QString, QVariant> it(whileCapturesMap);
        while (it.hasNext()) {
            it.next();
            QMap<QString, QVariant> m = it.value().toMap();
            rule.whileCaptures.insert(it.key().toInt(), makeRule(m, blockStateID));
        }
    }

    return rule;
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

    int startId = 0;
    rule = this->makeRule(map, startId);

    keywordFormat.setForeground(Qt::darkMagenta);
    // multiLineFormat.setForeground(Qt::green);
    multiLineFormat.setForeground(QColor(191, 255, 0, 255));
}
