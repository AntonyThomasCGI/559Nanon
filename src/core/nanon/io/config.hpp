#pragma once

#include <QtCore/QString>
#include <QtCore/QVariant>

namespace nanon {
namespace io {

struct ConfigParseError {
    enum ParseError {NoError, IOFailure, InvalidJson, InvalidPList};
    ConfigParseError::ParseError error;

    ConfigParseError() : error(ConfigParseError::NoError) {}

    QString errorString;
};


/**
 * A generic config file parser that takes a json or xml file and returns
 * a QVariant containing the raw data.
 */
class ConfigParser {

public:
    ConfigParser();
    virtual ~ConfigParser();

    QVariant parse(QString filename, ConfigParseError &err);

    QVariant parsePList(QString filename, ConfigParseError &err);

    QVariant parseJSON(QString filename, ConfigParseError &err);
};

};  // namespace io
};  // namespace nanon
