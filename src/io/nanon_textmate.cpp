
#include "io/nanon_textmate.hpp"

#include <iostream>

#include <QtCore/QJsonParseError>
#include <QtCore/QFile>


TextMateParser::TextMateParser()
{}

TextMateParser::~TextMateParser()
{}

QVariant TextMateParser::parse(QString filename, TextMateParseError &err)
{
    if (filename.endsWith(QString(".json"))) {
        return parseJSON(filename, err);
    } else {
        return parseXML(filename, err);
    }
}

QVariant TextMateParser::parseJSON(QString filename, TextMateParseError &err)
{
    QFile file;
    file.setFileName(filename);
    if (!file.open(QIODevice::ReadOnly))
    {
        err.errorString = QString("Failed to open file: ") + filename;
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


QVariant TextMateParser::parseXML(QString filename, TextMateParseError &err)
{
    std::cout << "parseXML not implemented!" << std::endl;

    return QVariant();
}




// std::optional<QJsonDocument> parseJsonFile(QString fileName)
// {
//     QFile file;
//     file.setFileName(fileName);
//     if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
//         std::cout << "WARN: Failed to open file!" << std::endl;
//         return {};
//     }

//     QJsonParseError jsonError;
//     QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &jsonError);
//     if (jsonError.error != QJsonParseError::NoError) {
//         std::cout << "WARN: Could not parse json file: " << qUtf8Printable(jsonError.errorString()) << std::endl;
//         file.close();
//         return {};
//     };
//     file.close();

//     return doc;
// }
