#ifndef REDACT_HPP_INCLUDED
#define REDACT_HPP_INCLUDED

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPlainTextEdit>


class NanonEditor : public QMainWindow
{
    Q_OBJECT

public:
    NanonEditor(QWidget* parent = 0);
    virtual ~NanonEditor();

private:
    void createStatusBar();

    // QHBoxLayout    *layout;
    // QPlainTextEdit *outputWindow;
    // QPlainTextEdit *editor;
};

#endif
