#pragma once

#include <QtCore/QString>
#include <QtCore/QVariant>

struct TextMateParseError {
    enum ParseError {NoError, IOFailure, InvalidJson};
    TextMateParseError::ParseError error;

    TextMateParseError() : error(TextMateParseError::NoError) {}

    QString errorString;
};


class TextMateParser {

public:
    TextMateParser();
    virtual ~TextMateParser();

    QVariant parse(QString filename, TextMateParseError &err);

    QVariant parseXML(QString filename, TextMateParseError &err);

    QVariant parseJSON(QString filename, TextMateParseError &err);
};

