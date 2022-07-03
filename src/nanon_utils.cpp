
#include "nanon_utils.hpp"

#include <iostream>

#include <QJsonParseError>
#include <QFile>




std::optional<QJsonDocument> parseJsonFile(QString fileName)
{
    QFile file;
    file.setFileName(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        std::cout << "WARN: Failed to open file!" << std::endl;
        return {};
    }

    QJsonParseError jsonError;

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &jsonError);
    if (jsonError.error != QJsonParseError::NoError) {
        std::cout << "WARN: Could not parse json file: " << qUtf8Printable(jsonError.errorString()) << std::endl;
        file.close();
        return {};
    };
    file.close();

    return doc;
}
