#include "nanon.hpp"

#include <QtWidgets/QStatusBar>
#include <QtWidgets/QHBoxLayout>


NanonEditor::NanonEditor(QWidget* parent)
{
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setSpacing(0);

    QPlainTextEdit *outputWindow = new QPlainTextEdit;
    outputWindow->setReadOnly(true);

    QPlainTextEdit *editor = new QPlainTextEdit;
    editor->setWordWrapMode(QTextOption::NoWrap);

    layout->addWidget(outputWindow);
    layout->addWidget(editor);

    setCentralWidget(new QWidget);
    centralWidget()->setLayout(layout);

    createStatusBar();
}

NanonEditor::~NanonEditor()
{}

void NanonEditor::createStatusBar()
{
    statusBar()->showMessage("Ready");
}
