#ifndef REDACT_HPP_INCLUDED
#define REDACT_HPP_INCLUDED

#include <QtWidgets/QPlainTextEdit>


class NanonEditor : public QPlainTextEdit
{
	Q_OBJECT

public:
			NanonEditor(QWidget* parent = 0);
	virtual	~NanonEditor();

};

#endif
