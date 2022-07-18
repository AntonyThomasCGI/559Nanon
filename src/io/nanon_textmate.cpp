
#include "io/nanon_textmate.hpp"

#include <iostream>

#include <QtCore/QFile>
#include <QtCore/QJsonParseError>
#include <QtCore/QXmlStreamReader>


TextMateParser::TextMateParser()
{}

TextMateParser::~TextMateParser()
{}

QVariant TextMateParser::parse(QString filename, TextMateParseError &err)
{
    if (filename.endsWith(".json")) {
        return parseJSON(filename, err);
    } else {
        return parsePList(filename, err);
    }
}

QVariant TextMateParser::parseJSON(QString filename, TextMateParseError &err)
{
    QFile file;
    file.setFileName(filename);
    if (!file.open(QIODevice::ReadOnly))
    {
        err.errorString = "Failed to open file: " + filename;
        err.error = TextMateParseError::ParseError::IOFailure;
        return QVariant();
    }
    QByteArray bytesIn = file.readAll();

    QJsonParseError jsonErr;
    QJsonDocument doc = QJsonDocument::fromJson(bytesIn, &jsonErr);
    if (jsonErr.error != QJsonParseError::NoError) {
        err.errorString = jsonErr.errorString();
        err.error = TextMateParseError::ParseError::InvalidJson;
    }
    file.close();

    return doc.toVariant();
}


QVariant parseRecursive(QXmlStreamReader *xmlRead)
{
    if (xmlRead->atEnd() || xmlRead->hasError()) {
        return QVariant();
    }

    QXmlStreamReader::TokenType token = xmlRead->readNext();

    if (token == QXmlStreamReader::StartDocument || token == QXmlStreamReader::DTD || token == QXmlStreamReader::Characters)
        return parseRecursive(xmlRead);
    if (token == QXmlStreamReader::EndElement)
        return QVariant();

    if (token == QXmlStreamReader::StartElement) {
        if (xmlRead->name() == "string" || xmlRead->name() == "key") {
            return QString(xmlRead->readElementText());
        }
        if (xmlRead->name() == "array") {
            QList<QVariant> arrayResult;
            QVariant arrayValue;
            while ((arrayValue = parseRecursive(xmlRead)).isValid()) {
                arrayResult.append(arrayValue);
            }
            return arrayResult;
        }
        if (xmlRead->name() == "dict") {
            QMap<QString, QVariant> dictResult;
            QVariant key;
            while ((key = parseRecursive(xmlRead)).isValid()) {
                QVariant value = parseRecursive(xmlRead);
                dictResult.insert(key.toString(), value);
            }
            return dictResult;
        }
        return parseRecursive(xmlRead);
    }
    return QVariant();
}



QVariant TextMateParser::parsePList(QString filename, TextMateParseError &err)
{
    QFile file;
    file.setFileName(filename);
    if (!file.open(QIODevice::ReadOnly))
    {
        err.errorString = "Failed to open file: " + filename;
        err.error = TextMateParseError::ParseError::IOFailure;
        return QVariant();
    }
    QXmlStreamReader *xmlRead = new QXmlStreamReader(&file);

    QVariant result = parseRecursive(xmlRead);
    if (xmlRead->hasError()) {
        err.errorString = "Failed to parse plist";
        err.error = TextMateParseError::ParseError::InvalidPList;
        return QVariant();
    }

    return result;
}
