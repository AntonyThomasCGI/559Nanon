
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


// void readString(QXmlStreamReader *xmlRead)
// {
//     QString res = xmlRead->readElementText();
// }


QVariant parseRecursive(QXmlStreamReader *xmlRead)
{
    // std::cout << "parseRecursive" << std::endl;
    if (xmlRead->atEnd() || xmlRead->hasError()) {
        // std::cout << "END OR ERROR" << std::endl;
        return QVariant();
    }

    QXmlStreamReader::TokenType token = xmlRead->readNext();
    // std::cout << "token type: " << qUtf8Printable(xmlRead->tokenString()) << std::endl;

    if (token == QXmlStreamReader::StartDocument || token == QXmlStreamReader::DTD || token == QXmlStreamReader::Characters)
        return parseRecursive(xmlRead);
    if (token == QXmlStreamReader::EndElement)
        return QVariant();

    if (token == QXmlStreamReader::StartElement) {
        // std::cout << "se type: " << qUtf8Printable(xmlRead->name().toString()) << std::endl;
        if (xmlRead->name() == "string" || xmlRead->name() == "key") {
            return QString(xmlRead->readElementText());
        }
        if (xmlRead->name() == "array") {
            QList<QVariant> arrayResult;
            // QVariant arrayValue;
            while (true) {
            // while ((arrayValue = parseRecursive(xmlRead)).isValid()) {
                // std::cout << "start parse array" << std::endl;
                QVariant arrayValue = parseRecursive(xmlRead);
                // std::cout << "got array item " << std::endl;
                if (arrayValue.isNull()) {
                    // std::cout << "break array!" << std::endl;
                    break;
                }
                // std::cout << "append value" << std::endl;
                arrayResult.append(arrayValue);
            }
            return arrayResult;
        }
        if (xmlRead->name() == "dict") {
            // std::cout << "in dict" << std::endl;
            QMap<QString, QVariant> dictResult;
            while (true) {
                QVariant key = parseRecursive(xmlRead);
                if (key.isNull()) {
                    // std::cout << "break dict!" << std::endl;
                    break;
                }
                QVariant value = parseRecursive(xmlRead);
                // std::cout << "key: " << qUtf8Printable(key.toString()) << std::endl;
                // // std::cout << "value: " << qUtf8Printable(value.toString()) << std::endl;
                // if (key.isNull() || value.isNull()) {
                //     break;
                // }
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

    return result;
}

// void readNext(QString name) {
//     std::cout << "hi" << std::endl;
// }
