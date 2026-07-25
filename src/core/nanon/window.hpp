#pragma once

#include "nanon/interpreter/base.hpp"
#include "nanon/widgets/editor.hpp"
#include "nanon/widgets/highlighter.hpp"


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
    virtual ~NanonWindow();

	void appendOutput(QString text);

    void setInterpreter(interpreter::NanonInterpreterBase* interpreter);

private:
    void createStatusBar();
    void configurePalette();

	std::unique_ptr<QPlainTextEdit> m_outputWindow;
    std::unique_ptr<widgets::NanonEditor> m_editor;

    interpreter::NanonInterpreterBase* m_interpreter = nullptr;

    void onRunCode();
    void onShowScopesAtCursor();

};

};  // namespace nanon
