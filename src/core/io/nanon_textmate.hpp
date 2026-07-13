#pragma once

#include <QtCore/QString>
#include <QtCore/QVariant>

struct TextMateParseError {
    enum ParseError {NoError, IOFailure, InvalidJson, InvalidPList};
    TextMateParseError::ParseError error;

    TextMateParseError() : error(TextMateParseError::NoError) {}

    QString errorString;
};


class TextMateParser {

public:
    TextMateParser();
    virtual ~TextMateParser();

    QVariant parse(QString filename, TextMateParseError &err);

    QVariant parsePList(QString filename, TextMateParseError &err);

    QVariant parseJSON(QString filename, TextMateParseError &err);
};
