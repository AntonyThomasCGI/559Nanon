#pragma once

#include "nanon/interpreter/base.hpp"
#include "nanon/widgets/editor.hpp"
#include "nanon/widgets/highlighter.hpp"
#include "nanon/widgets/tab_bar.hpp"
#include "nanon/session.hpp"

#include <QtCore/QPointer>
#include <QtCore/QString>
#include <QtGui/QSyntaxHighlighter>
#include <QtGui/QTextDocument>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPlainTextEdit>

#include <memory>


namespace nanon {


class NanonWindow : public QMainWindow
{
    Q_OBJECT

public:
    NanonWindow(QWidget* parent = 0);
    virtual ~NanonWindow() = default;

    void appendOutput(QString text);

    void setInterpreter(interpreter::NanonInterpreterBase* interpreter);

private:
    void createStatusBar();
    void configurePalette();

    std::unique_ptr<NanonSession> m_session;

    QPlainTextEdit* m_outputWindow;
    widgets::NanonEditor* m_editor;
    widgets::NanonTabWidget* m_tabWidget;

    interpreter::NanonInterpreterBase* m_interpreter = nullptr;

    void onChooseColorTheme();
    void onClearOutput();
    void onRunCode();
    void onShowCommandPalette();
    void onShowScopesAtCursor();

};

};  // namespace nanon
