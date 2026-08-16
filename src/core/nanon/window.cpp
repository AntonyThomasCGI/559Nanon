
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

    m_session = QSharedPointer<NanonSession>::create();

    QSplitter *splitter = new QSplitter(Qt::Vertical);

    m_outputWindow = new QPlainTextEdit(this);
    m_outputWindow->setReadOnly(true);
    m_outputWindow->setFrameShape(QFrame::NoFrame);
    // TODO, wrap mode setting?
    m_outputWindow->setWordWrapMode(QTextOption::WrapAnywhere);

    m_editor = new widgets::NanonEditor(m_session, this);
    m_editor->setWordWrapMode(QTextOption::NoWrap);
    m_editor->setFrameShape(QFrame::NoFrame);

    splitter->addWidget(m_outputWindow);
    splitter->addWidget(m_editor);

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
        m_editor->setFont(font);
        m_outputWindow->setFont(font);
    } else {
        std::cerr << "Failed to load font: " << defaultFont.string() << std::endl;
    }

    // Load icons
    std::filesystem::path mainIconPath = resourcePath / "icons" / "nanon_icon_1024.png";

    setWindowIcon(QIcon(mainIconPath.c_str()));

    createStatusBar();

    configurePalette();
    connect(m_session.get(), &NanonSession::themeChanged, this, &NanonWindow::configurePalette);

    // Actions:
    auto changeThemeAction = new commands::NanonAction("Set Color Theme", this);
    connect(changeThemeAction, &commands::NanonAction::triggered, this, &NanonWindow::onChooseColorTheme);

    auto runCodeAction = new commands::NanonAction("Run Code", QKeySequence(Qt::META | Qt::Key_Return), this);
    connect(runCodeAction, &commands::NanonAction::triggered, this, &NanonWindow::onRunCode);

    auto showScopesAction = new commands::NanonAction("Show Scopes At Cursor", this);
    connect(showScopesAction, &commands::NanonAction::triggered, this, &NanonWindow::onShowScopesAtCursor);

    auto commandPaletteAction = new commands::NanonAction("Command Palette", QKeySequence(Qt::META | Qt::SHIFT | Qt::Key_P), this, true);
    connect(commandPaletteAction, &commands::NanonAction::triggered, this, &NanonWindow::onShowCommandPalette);

    auto clearOutputAction = new commands::NanonAction("Clear Output", this);
    connect(clearOutputAction, &commands::NanonAction::triggered, this, &NanonWindow::onClearOutput);

    connect(m_editor, &widgets::NanonEditor::tabCountChanged, this, &NanonWindow::onTabCountChanged);
    connect(m_editor, &widgets::NanonEditor::tabChanged, this, &NanonWindow::onTabChanged);

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

    m_editor->saveEditorSession();
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
    // TODO, this is scuffed
    m_tabStatus = new QLabel("/");
    statusBar()->addWidget(m_tabStatus);
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


void NanonWindow::onTabCountChanged(int newCount)
{
    QString text = m_tabStatus->text();
    QStringList splitText = text.split("/");
    splitText[1] = QString::number(newCount);
    m_tabStatus->setText(splitText[0] + "/" + splitText[1]);
}


void NanonWindow::onTabChanged(int newIndex)
{
    QString text = m_tabStatus->text();
    QStringList splitText = text.split("/");
    splitText[0] = QString::number(newIndex);
    m_tabStatus->setText(splitText[0] + "/" + splitText[1]);
}
