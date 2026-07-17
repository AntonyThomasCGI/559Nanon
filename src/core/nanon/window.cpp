
#include "nanon/window.hpp"
#include "nanon/textmate/rule.hpp"

#include <QFont>
#include <QFontDatabase>
#include <QShortcut>
#include <QtWidgets/QMenu>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QStatusBar>

#include <filesystem>
#include <iostream>
#include <string>


using namespace nanon;


NanonWindow::NanonWindow(QWidget* parent)
{
    this->setWindowTitle("559 Nanon");
    this->setStyleSheet("background-color:white;");

    QSplitter *splitter = new QSplitter(Qt::Vertical);

    outputWindow = new QPlainTextEdit;
    outputWindow->setReadOnly(true);
    outputWindow->setWordWrapMode(QTextOption::NoWrap);
    outputWindow->setStyleSheet("background-color:black; color:Gainsboro");

    editor = new widgets::NanonEditor;
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

    highlighter = new widgets::Highlighter(editor->document());

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

    interpreter::ExecutionResult result = m_interpreter->executeCode(strContent);

    QString resultStdout = QString::fromStdString(result.stdout);
    QString resultStderr = QString::fromStdString(result.stderr);

    appendOutput(resultStdout);
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
    // Append to end of output
    this->outputWindow->moveCursor (QTextCursor::End);
    this->outputWindow->insertPlainText(text);
    // Force re-scroll to the bottom
    this->outputWindow->moveCursor (QTextCursor::End);
}


void NanonWindow::setInterpreter(interpreter::NanonInterpreterBase* interpreter)
{
    m_interpreter = interpreter;
}

