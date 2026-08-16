#pragma once

#include "nanon/interpreter/base.hpp"
#include "nanon/widgets/editor.hpp"
#include "nanon/widgets/highlighter.hpp"
#include "nanon/session.hpp"

#include <QtCore/QPointer>
#include <QtCore/QSharedPointer>
#include <QtCore/QString>
#include <QtGui/QSyntaxHighlighter>
#include <QtGui/QTextDocument>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QLabel>


namespace nanon {


class NanonWindow : public QMainWindow
{
    Q_OBJECT

public:
    NanonWindow(QWidget* parent = 0);
    virtual ~NanonWindow() = default;

    void appendOutput(QString text);

    void setInterpreter(interpreter::NanonInterpreterBase* interpreter);


protected slots:

    void onTabCountChanged(int newCount);
    void onTabChanged(int newIndex);

private:
    void createStatusBar();
    void configurePalette();

    QSharedPointer<NanonSession> m_session;

    QLabel *m_tabStatus;

    QPlainTextEdit* m_outputWindow;
    widgets::NanonEditor* m_editor;

    interpreter::NanonInterpreterBase* m_interpreter = nullptr;

    void onChooseColorTheme();
    void onClearOutput();
    void onRunCode();
    void onShowCommandPalette();
    void onShowScopesAtCursor();

};

};  // namespace nanon
