
#include "nanon/io/config.hpp"
#include "nanon/style/theme.hpp"
#include "nanon/style/theme_manager.hpp"
#include "nanon/textmate/rule.hpp"
#include "nanon/window.hpp"

#include <QFont>
#include <QFontDatabase>
#include <QShortcut>
#include <QtGui/QTextDocumentFragment>
#include <QtWidgets/QMenu>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QStatusBar>

#include <filesystem>
#include <iostream>
#include <string>


#ifndef RESOURCE_PATH
#define RESOURCE_PATH = ""
#endif


using namespace nanon;


NanonWindow::NanonWindow(QWidget* parent)
{
    this->setWindowTitle("559 Nanon");

    std::filesystem::path resourcePath = RESOURCE_PATH;
    std::filesystem::path themePath = resourcePath / "themes" / "nanon-theme.json";

    auto& themeManager = style::NanonThemeManager::instance();
    themeManager.addTheme("Nanon Theme", themePath.c_str());
    style::NanonTheme* theme = themeManager.theme();

    QSplitter *splitter = new QSplitter(Qt::Vertical);

    m_outputWindow = std::make_unique<QPlainTextEdit>();
    m_outputWindow->setReadOnly(true);
    m_outputWindow->setWordWrapMode(QTextOption::NoWrap);

    m_editor = std::make_unique<widgets::NanonEditor>();
    m_editor->setWordWrapMode(QTextOption::NoWrap);

    splitter->addWidget(m_outputWindow.get());
    splitter->addWidget(m_editor.get());

    setCentralWidget(splitter);

    m_editor->setFocus();

    // Load fonts
    std::filesystem::path fontPath = resourcePath / "fonts";

    std::filesystem::path defaultFont = fontPath / "Courier_Prime" / "CourierPrime-Regular.ttf";

    std::cout << "Loading font: " << defaultFont << std::endl;
    int fontId = QFontDatabase::addApplicationFont(defaultFont.string().c_str());

    QStringList fontFamilies = QFontDatabase::applicationFontFamilies(fontId);
    if (!fontFamilies.empty()) {
        std::cout << "Loaded font: " << fontFamilies.at(0).toStdString() << std::endl;
        QString fontFamily = fontFamilies.at(0);
        this->setFont(QFont(fontFamily));
        m_editor->setFont(QFont(fontFamily));
        m_outputWindow->setFont(QFont(fontFamily));
    } else {
        std::cerr << "Failed to load font: " << defaultFont.string() << std::endl;
    }

    createStatusBar();

    configurePalette();

    QShortcut *shortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_Return), this);
    connect(shortcut, &QShortcut::activated, this, &NanonWindow::onRunCode);

    QShortcut *scopesShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_S), this);
    connect(scopesShortcut, &QShortcut::activated, this, &NanonWindow::onShowScopesAtCursor);
}


NanonWindow::~NanonWindow()
{}


void NanonWindow::configurePalette()
{
    style::NanonTheme *theme = style::NanonThemeManager::instance().theme();
    if (theme == nullptr) {
        return;
    }

    QPalette outputPalette;

    outputPalette.setColor(
        QPalette::Highlight,
        QColor(theme->getColor("outputSelection.background"))
    );
    outputPalette.setColor(
        QPalette::HighlightedText,
        QColor(theme->getColor("output.foreground"))
    );
    outputPalette.setColor(
        QPalette::Text,
        QColor(theme->getColor("output.foreground"))
    );
    outputPalette.setColor(
        QPalette::Base,
        QColor(theme->getColor("output.background"))
    );

    m_outputWindow->setPalette(outputPalette);

    QPalette statusPalette;

    statusPalette.setColor(
        QPalette::Window,
        QColor(theme->getColor("statusBar.background"))
    );
    statusPalette.setColor(
        QPalette::WindowText,
        QColor(theme->getColor("statusBar.foreground"))
    );

    statusBar()->setPalette(statusPalette);

    QPalette windowPalette;

    windowPalette.setColor(
        QPalette::Window,
        QColor(theme->getColor("window.background"))
    );

    setPalette(windowPalette);
}


void NanonWindow::onRunCode()
{
    if (m_interpreter == nullptr) {
        std::cout << "WARNING no interpreter set" << std::endl;
        return;
    }

    QString content;
    if (m_editor->textCursor().hasSelection()) {
        content = m_editor->textCursor().selection().toPlainText();
    } else {
        content = m_editor->toPlainText();
    }

    std::string strContent = content.toStdString();
    interpreter::ExecutionResult result = m_interpreter->executeCode(strContent);

    QString resultStdout = QString::fromStdString(result.stdout);
    QString resultStderr = QString::fromStdString(result.stderr);

    appendOutput(resultStdout);
}


void NanonWindow::onShowScopesAtCursor()
{
    QTextCursor cursor = m_editor->textCursor();
    QTextBlock currentBlock = cursor.block();
    if (!currentBlock.isValid()) {
        return;
    }

    int pos = cursor.positionInBlock();

    QVector<QString> scopes = m_editor->scopesAtPosition(currentBlock, pos);

    const QPoint cursorCoordinates = m_editor->cursorRect().bottomRight();
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
    menu.exec(m_editor->viewport()->mapToGlobal(cursorCoordinates));
}



void NanonWindow::createStatusBar()
{
    statusBar()->showMessage("Ready");
}


void NanonWindow::appendOutput(QString text)
{
    // Append to end of output
    m_outputWindow->moveCursor (QTextCursor::End);
    m_outputWindow->insertPlainText(text);
    // Force re-scroll to the bottom
    m_outputWindow->moveCursor (QTextCursor::End);
}


void NanonWindow::setInterpreter(interpreter::NanonInterpreterBase* interpreter)
{
    m_interpreter = interpreter;
}

