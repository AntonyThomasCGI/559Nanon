
#include "nanon.hpp"

#include <iostream>
#include <vector>
#include <string>

#include <QtCore/QRegularExpressionMatchIterator>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QSplitter>
#include <QtGui/QPainter>
#include <QtGui/QTextBlock>
#include <QtCore/QTimer>


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
    highlighter = new Highlighter(editor->document());

    splitter->addWidget(outputWindow);
    splitter->addWidget(editor);

    setCentralWidget(splitter);
    // centralWidget()->setLayout(layout);

    createStatusBar();

    QString tempText = R""""(def hi():
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


// void Highlighter::reformatBlocks(int from, int charsRemoved, int charsAdded)
// {
//     QTextBlock block = doc->findBlock(from);
//     if (!block.isValid())
//         return;

//     int endPosition;
// }

// void Highlighter::setDocument(QTextDocument *doc)
// {
//     if (doc) {
//         disconnect(doc, SIGNAL(contentsChange(int,int,int)),
//                    this, SLOT(reformatBlocks(int,int,int)));
//         QTextCursor cursor(doc);
//         cursor.beginEditBlock();
//         for (QTextBlock blk = doc->begin(); blk.isValid(); blk = blk.next())
//             blk.layout()->clearFormats();
//         cursor.endEditBlock();
//     }
//     doc = doc;
//     // if (doc) {
//     //     connect(doc, SIGNAL(contentsChange(int,int,int)),
//     //             this, SLOT(_q_reformatBlocks(int,int,int)));
//     //     if (!doc->isEmpty()) {
//     //         // this->rehighlightPending = true;
//     //         QTimer::singleShot(0, this, SLOT(_q_delayedRehighlight()));
//     //     }
//     // }
// }

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
    // for (const HighlightingRule &rule : qAsConst(highlightingRules)) {
    //     QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
    //     while (matchIterator.hasNext()) {
    //         QRegularExpressionMatch match = matchIterator.next();
    //         setFormat(match.capturedStart(), match.capturedLength(), rule.format);
    //     }
    // }

    for (Rule & pattern : rule.patterns) {
        if (pattern.match != QRegularExpression{}) {
            QRegularExpressionMatchIterator matchIterator = pattern.match.globalMatch(text);
            while (matchIterator.hasNext()) {
                QRegularExpressionMatch match = matchIterator.next();
                setFormat(match.capturedStart(), match.capturedLength(), keywordFormat);
            }
        }
    }

    // QVector<QString>::iterator scopeIt;
    // // QVector<QString> toRemove;
    // for (scopeIt = scopes.begin(); scopeIt != scopes.end(); ++scopeIt) {
    //     QString scopeName = *scopeIt;
    //     for (Rule & pattern : rule.patterns) {
    //         if (pattern.name == scopeName) {  // Found pattern for existing scope.
    //             // Try to match end of scope.
    //             QRegularExpressionMatch match = pattern.end.match(text);
    //             int endIndex = match.capturedStart();
    //             if (endIndex > -1) {
    //                 // End of capture found, remove from scopes.
    //                 int scopeLength = endIndex + match.capturedLength();
    //                 setFormat(0, scopeLength, multiLineFormat);
    //                 // scopeIt = scopes.erase(scopeIt);
    //                 // toRemove.push_back(scopeName);
    //             } else {
    //                 // Not found, set whole line.
    //                 setFormat(0, text.length(), multiLineFormat);
    //             }
    //             break;
    //         }
    //     }
    // }
    // if (scopes.length()) {
    //     QVector<QString>::iterator scopeIt;
    //     for (scopeIt = scopes.begin(); scopeIt != scopes.end(); ++scopeIt) {
    //         scopeIt = scopes.erase(scopeIt);
    //     }
    // }
    // QTextBlock thisBlock = currentBlock();
    ScopeBlockData *scopeData = previousBlockUserData();
    QVector<QString> scopes;
    if (scopeData != NULL) {
        scopes = scopeData->scopes;
    }

    ScopeBlockData *nextScopeData = new ScopeBlockData;
    std::cout << "copying" << std::endl;
    for (QString str : scopes) {
        nextScopeData->scopes.push_back(str);
    }
    std::cout << "done copying" << std::endl;

    // Try to match non existing scopes.
    for (Rule & pattern : rule.patterns) {
        if (pattern.begin != QRegularExpression{} && pattern.end != QRegularExpression{} && pattern.name != QString{}) {
            int startIndex = 0;

            if (scopes.contains(pattern.name)) {  // We are already in scope.
                QRegularExpressionMatch match = pattern.end.match(text, startIndex);
                int endIndex = match.capturedStart();
                if (endIndex > -1) {
                    // End of capture found, remove from next scopes.
                    setFormat(startIndex, endIndex + 1, multiLineFormat);

                    startIndex = endIndex + 1;

                    // if scopes.contains()
                    std::cout << "REMOVE1" << std::endl;
                    nextScopeData->scopes.removeAt(scopes.indexOf(pattern.name));
                } else {
                    // Not found, set whole line.
                    setFormat(startIndex, text.length(), multiLineFormat);
                    startIndex = text.length();

                    // Check case where we found end but new scope started on same line.
                    // if (!nextScopeData->scopes.contains(pattern.name)) {
                    //     std::cout << "ADD1" << std::endl;
                    //     nextScopeData->scopes.push_back(pattern.name);
                    // }
                }
            }
            printf("startI: %d\n", startIndex);

            startIndex = text.indexOf(pattern.begin, startIndex);
            while (startIndex >= 0) {
                // printf("start index %d\n", startIndex);
                QRegularExpressionMatch match = pattern.end.match(text, startIndex + 1);
                int endIndex = match.capturedStart();
                // printf("end index %d\n", endIndex);
                int capturedLength = 0;
                if (endIndex == -1) {  // End is not in this block, add to scopes for next block.
                    capturedLength = text.length() - startIndex;
                    std::cout << "ADD2" << std::endl;
                    nextScopeData->scopes.push_back(pattern.name);
                } else {  // End of capture found, remove from scopes.
                    capturedLength = endIndex - startIndex + match.capturedLength();
                }
                setFormat(startIndex, capturedLength, multiLineFormat);
                startIndex = text.indexOf(pattern.begin, startIndex + capturedLength);
            }
        }
    }

    // std::cout << "looping" << std::endl;

    setCurrentBlockUserData(nextScopeData);

    // for (int i = 0; i < scopes.size(); ++i) {
    //     scopes
    // }
    // setCurrentBlockState(0);

    // int startIndex = 0;
    // if (previousBlockState() != 1)
    //     startIndex = text.indexOf(commentStartExpression);

    // while (startIndex >= 0) {
    //     QRegularExpressionMatch match = commentEndExpression.match(text, startIndex);
    //     int endIndex = match.capturedStart();
    //     int commentLength = 0;
    //     if (endIndex == -1) {
    //         setCurrentBlockState(1);
    //         commentLength = text.length() - startIndex;
    //     } else {
    //         commentLength = endIndex - startIndex
    //                         + match.capturedLength();
    //     }
    //     setFormat(startIndex, commentLength, multiLineFormat);
    //     startIndex = text.indexOf(commentStartExpression, startIndex + commentLength);
    // }
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
            rule.captures.push_back(makeRule(m, blockStateID));
        }
    }

    if (map.contains("beginCaptures")) {
        QMap<QString, QVariant> beginCapturesMap = map.value("beginCaptures").toMap();
        QMapIterator<QString, QVariant> it(beginCapturesMap);
        while (it.hasNext()) {
            it.next();
            QMap<QString, QVariant> m = it.value().toMap();
            rule.beginCaptures.push_back(makeRule(m, blockStateID));
        }
    }

    if (map.contains("endCaptures")) {
        QMap<QString, QVariant> endCapturesMap = map.value("endCaptures").toMap();
        QMapIterator<QString, QVariant> it(endCapturesMap);
        while (it.hasNext()) {
            it.next();
            QMap<QString, QVariant> m = it.value().toMap();
            rule.endCaptures.push_back(makeRule(m, blockStateID));
        }
    }

    if (map.contains("whileCaptures")) {
        QMap<QString, QVariant> whileCapturesMap = map.value("whileCaptures").toMap();
        QMapIterator<QString, QVariant> it(whileCapturesMap);
        while (it.hasNext()) {
            it.next();
            QMap<QString, QVariant> m = it.value().toMap();
            rule.whileCaptures.push_back(makeRule(m, blockStateID));
        }
    }

    return rule;
}


void Highlighter::setSyntaxFromFile(QString fileName)
{
    TextMateParseError err;
    std::cout << std::endl << "=== start ===" << std::endl;

    TextMateParser tmParser = TextMateParser();
    QVariant tmData = tmParser.parse(fileName, err);
    if (err.error != TextMateParseError::ParseError::NoError) {
        std::cout << "ERROR: " << qUtf8Printable(err.errorString) << std::endl;
        return;
    }

    QMap<QString, QVariant> map = tmData.toMap();
    // std::cout << qUtf8Printable(map["scopeName"].toString()) << std::endl;
    // Rule rule = makeRule(map);

    // std::cout << std::endl << "--ok print entire map--" << std::endl;


    // QMapIterator<QString, QVariant> i(map);
    // while (i.hasNext()) {
    //     i.next();
    //     if (i.key() == "a") {
    //         std::cout << qUtf8Printable(i.key()) << ": "<< qUtf8Printable(i.value().toString()) << std::endl;
    //     } else if (i.key() == "patterns") {
    //         QList<QVariant> allPatterns = i.value().toList();
    //         for (int j = 0; j < allPatterns.size(); ++j) {
    //             QMap<QString, QVariant> pattern = allPatterns.at(j).toMap();
    //             QMapIterator<QString, QVariant> k(pattern);
    //             while (k.hasNext()) {
    //                 k.next();
    //                 if (k.key() == "match") {
    //                     std::cout << "    " << qUtf8Printable(k.key()) << ": " << qUtf8Printable(k.value().toString()) << std::endl;
    //                 }
    //             }
    //         }
    //     }
    // }

    int startId = 0;
    rule = this->makeRule(map, startId);

    // HighlightingRule highlightRule;


    // std::vector<QString> kwPatterns;
    // std::vector<Rule> toParse = rule.patterns;
    // std::cout << "=======================" << std::endl;

    // while (toParse.size())
    // {
    //     Rule thisRule = toParse.back();
    //     std::cout << qUtf8Printable(thisRule.match) << std::endl;
    //     if (thisRule.match != QString{}) {
    //         kwPatterns.push_back(thisRule.match);
    //     }

    //     toParse.pop_back();

    //     if (thisRule.patterns.size()) {
    //         for (auto & pattern : thisRule.patterns) {
    //             toParse.push_back(pattern);
    //         }
    //     }
    // }

    //  = {
    //     QStringLiteral("\\bdef\\b"),
    //     QStringLiteral("\\breturn\\b"),
    //     QStringLiteral("\\bif\\b"),
    //     QStringLiteral("\\belif\\b"),
    //     QStringLiteral("\\belse\\b"),
    //     QStringLiteral("\\bint\\b"),
    //     QStringLiteral("\\bstr\\b"),
    //     QStringLiteral("\\blist\\b"),
    // };
    keywordFormat.setForeground(Qt::darkMagenta);
    multiLineFormat.setForeground(Qt::green);

    // for (auto & pattern : kwPatterns) {
    //     highlightRule.pattern = QRegularExpression(pattern);
    //     highlightRule.format = keywordFormat;
    //     highlightingRules.append(highlightRule);
    // }


    // commentStartExpression = QRegularExpression(QStringLiteral("/\\*"));
    // commentEndExpression = QRegularExpression(QStringLiteral("\\*/"));

    // multiLineCommentFormat.setForeground(Qt::red);



    // std::cout << "name: " << qUtf8Printable(rule.name) << std::endl;
    // std::cout << "match: " << qUtf8Printable(rule.match) << std::endl;
    // std::cout << "begin: " << qUtf8Printable(rule.begin) << std::endl;
    // std::cout << "end: " << qUtf8Printable(rule.end) << std::endl;
    // std::cout << "while: " << qUtf8Printable(rule.while_) << std::endl;
    // std::cout << "include: " << qUtf8Printable(rule.include) << std::endl;

    // for (auto & pattern : rule.patterns) {
    //     std::cout << "    pattern name: " << qUtf8Printable(pattern.name) << std::endl;
    //     std::cout << "    name: " << qUtf8Printable(pattern.name) << std::endl;
    //     std::cout << "    match: " << qUtf8Printable(pattern.match) << std::endl;
    //     std::cout << "    begin: " << qUtf8Printable(pattern.begin) << std::endl;
    //     std::cout << "    end: " << qUtf8Printable(pattern.end) << std::endl;
    //     std::cout << "    while: " << qUtf8Printable(pattern.while_) << std::endl;
    //     std::cout << "    include: " << qUtf8Printable(pattern.include) << std::endl;
    //     for (auto & capture : pattern.captures) {
    //         std::cout << "        cap name: " << qUtf8Printable(capture.name) << std::endl;
    //     }
    // }

    // std::cout << std::endl << "--------------------------------------" << std::endl;
}
