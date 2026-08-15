
#include "nanon/commands/action.hpp"
#include "nanon/commands/registry.hpp"
#include "nanon/io/config.hpp"
#include "nanon/style/theme.hpp"
#include "nanon/style/theme_manager.hpp"
#include "nanon/textmate/rule.hpp"
#include "nanon/widgets/search_menu.hpp"
#include "nanon/window.hpp"

#include <QtGui/QStandardItemModel>
#include <QFile>
#include <QFont>
#include <QFontDatabase>
#include <QIcon>
#include <QShortcut>
#include <QStyle>
#include <QtGui/QTextDocumentFragment>
#include <QtWidgets/QMenu>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>

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

    m_session = std::make_unique<NanonSession>();

    QSplitter *splitter = new QSplitter(Qt::Vertical);

    m_outputWindow = new QPlainTextEdit(this);
    m_outputWindow->setReadOnly(true);
    m_outputWindow->setFrameShape(QFrame::NoFrame);
    // TODO, wrap mode setting?
    m_outputWindow->setWordWrapMode(QTextOption::WrapAnywhere);

    m_editor = new widgets::NanonEditor(m_session.get(), this);
    m_editor->setWordWrapMode(QTextOption::NoWrap);
    m_editor->setFrameShape(QFrame::NoFrame);

    m_tabWidget = new widgets::NanonTabWidget(this);
    m_tabWidget->setDocumentMode(true);
    m_tabWidget->setTabShape(QTabWidget::Rounded);

    m_tabWidget->addTab(m_editor, "default");

    auto editor1 = new widgets::NanonEditor(m_session.get(), this);
    editor1->setWordWrapMode(QTextOption::NoWrap);
    editor1->setFrameShape(QFrame::NoFrame);
    m_tabWidget->addTab(editor1, "another");

    auto editor2 = new widgets::NanonEditor(m_session.get(), this);
    editor2->setWordWrapMode(QTextOption::NoWrap);
    editor2->setFrameShape(QFrame::NoFrame);
    m_tabWidget->addTab(editor2, "another2");

    splitter->addWidget(m_outputWindow);
    splitter->addWidget(m_tabWidget);

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
        QFont font = QFont(fontFamily);
        font.setPointSize(14);
        this->setFont(font);
        m_editor->setFont(QFont(fontFamily));
        m_outputWindow->setFont(QFont(fontFamily));
    } else {
        std::cerr << "Failed to load font: " << defaultFont.string() << std::endl;
    }

    // Load icons
    std::filesystem::path mainIconPath = resourcePath / "icons" / "nanon_icon_1024.png";

    setWindowIcon(QIcon(mainIconPath.c_str()));

    createStatusBar();

    configurePalette();
    connect(m_session.get(), &NanonSession::themeChanged, this, &NanonWindow::configurePalette);

    QString editorText = m_session->loadEditorContent();
    m_editor->setPlainText(editorText);

    // Actions:
    auto changeThemeAction = new commands::NanonAction("Set Color Theme", this);
    connect(changeThemeAction, &commands::NanonAction::triggered, this, &NanonWindow::onChooseColorTheme);

    auto runCodeAction = new commands::NanonAction("Run Code", QKeySequence(Qt::CTRL | Qt::Key_Return), this);
    connect(runCodeAction, &commands::NanonAction::triggered, this, &NanonWindow::onRunCode);

    auto showScopesAction = new commands::NanonAction("Show Scopes At Cursor", this);
    connect(showScopesAction, &commands::NanonAction::triggered, this, &NanonWindow::onShowScopesAtCursor);

    auto commandPaletteAction = new commands::NanonAction("Command Palette", QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_P), this, true);
    connect(commandPaletteAction, &commands::NanonAction::triggered, this, &NanonWindow::onShowCommandPalette);

    auto clearOutputAction = new commands::NanonAction("Clear Output", this);
    connect(clearOutputAction, &commands::NanonAction::triggered, this, &NanonWindow::onClearOutput);
}


void NanonWindow::configurePalette()
{
    auto theme = m_session->currentTheme();
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

    QPalette tabPalette;
    tabPalette.setColor(
        QPalette::Window,
        QColor(theme->getColor("editor.background"))
    );
    tabPalette.setColor(
        QPalette::Base,
        QColor(theme->getColor("editor.background"))
    );
    tabPalette.setColor(
        QPalette::Button,
        QColor(theme->getColor("menu.foreground"))
    );
    tabPalette.setColor(
        QPalette::Highlight,
        QColor(theme->getColor("editorLineNumberSpecial.foreground"))
    );
    m_tabWidget->setPalette(tabPalette);
    //m_tabWidget->tabBar()->setProperty("drawBase", 0);
    m_tabWidget->tabBar()->setPalette(tabPalette);

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


void NanonWindow::onClearOutput()
{
    m_outputWindow->clear();
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

    m_session->saveEditorContent(m_editor->toPlainText());
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
    }
    if (!hasScope) {
        menu.addAction("Not in a scope");
    }
    menu.setPalette(palette());
    menu.exec(m_editor->viewport()->mapToGlobal(cursorCoordinates));
}


void NanonWindow::onShowCommandPalette()
{
    auto &registry = commands::NanonCommandRegistry::instance();
    auto actions = registry.getAllActions();

    widgets::SearchMenu *menu = new widgets::SearchMenu(actions);

    QPoint widgetCenter = rect().center();
    int menuTop = rect().top() + (widgetCenter.y() - rect().top()) / 2;
    QPoint topCenter = QPoint(widgetCenter.x(), menuTop);
    QPoint globalTopCenter = mapToGlobal(topCenter);
    globalTopCenter.rx() -= menu->sizeHint().width() / 2;

    menu->exec(globalTopCenter);

    menu->deleteLater();
}


// TODO, move this action to theme manager, have it create a new search menu with themes
void NanonWindow::onChooseColorTheme()
{
    auto &themeManager = style::NanonThemeManager::instance();
    QList<commands::NanonAction*> themeActions;

    for (auto &theme : themeManager.themes()) {
        auto action = new commands::NanonAction(theme.getName(), this, true);
        connect(action, &commands::NanonAction::triggered, this, [this, theme]() {
            auto *themePtr = style::NanonThemeManager::instance().getThemeByName(theme.getName());
            m_session->setCurrentTheme(themePtr);
        });
        themeActions.push_back(action);
    }

    widgets::SearchMenu *menu = new widgets::SearchMenu(themeActions);

    QPoint widgetCenter = rect().center();
    int menuTop = rect().top() + (widgetCenter.y() - rect().top()) / 2;
    QPoint topCenter = QPoint(widgetCenter.x(), menuTop);
    QPoint globalTopCenter = mapToGlobal(topCenter);
    globalTopCenter.rx() -= menu->sizeHint().width() / 2;

    menu->exec(globalTopCenter);

    menu->deleteLater();
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
