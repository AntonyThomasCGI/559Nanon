#include "nanon/io/config.hpp"

#include <QtCore/QFile>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonParseError>
#include <QtCore/QString>
#include <QtCore/QXmlStreamReader>

#include <iostream>

using namespace nanon::io;


ConfigParser::ConfigParser()
{}

ConfigParser::~ConfigParser()
{}

QVariant ConfigParser::parse(QString filename, ConfigParseError &err)
{
    if (filename.endsWith(".json")) {
        return parseJSON(filename, err);
    } else {
        return parsePList(filename, err);
    }
}

QVariant ConfigParser::parseJSON(QString filename, ConfigParseError &err)
{
    QFile file;
    file.setFileName(filename);
    if (!file.open(QIODevice::ReadOnly))
    {
        err.errorString = "Failed to open file: " + filename;
        err.error = ConfigParseError::ParseError::IOFailure;
        return QVariant();
    }
    QByteArray bytesIn = file.readAll();

    QJsonParseError jsonErr;
    QJsonDocument doc = QJsonDocument::fromJson(bytesIn, &jsonErr);
    if (jsonErr.error != QJsonParseError::NoError) {
        err.errorString = jsonErr.errorString();
        err.error = ConfigParseError::ParseError::InvalidJson;
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
        if (xmlRead->name() == u"string" || xmlRead->name() == u"key") {
            return QString(xmlRead->readElementText());
        }
        if (xmlRead->name() == u"array") {
            QList<QVariant> arrayResult;
            QVariant arrayValue;
            while ((arrayValue = parseRecursive(xmlRead)).isValid()) {
                arrayResult.append(arrayValue);
            }
            return arrayResult;
        }
        if (xmlRead->name() == u"dict") {
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



QVariant ConfigParser::parsePList(QString filename, ConfigParseError &err)
{
    QFile file;
    file.setFileName(filename);
    if (!file.open(QIODevice::ReadOnly))
    {
        err.errorString = "Failed to open file: " + filename;
        err.error = ConfigParseError::ParseError::IOFailure;
        return QVariant();
    }
    QXmlStreamReader *xmlRead = new QXmlStreamReader(&file);

    QVariant result = parseRecursive(xmlRead);
    if (xmlRead->hasError()) {
        err.errorString = "Failed to parse plist";
        err.error = ConfigParseError::ParseError::InvalidPList;
        return QVariant();
    }

    return result;
}
