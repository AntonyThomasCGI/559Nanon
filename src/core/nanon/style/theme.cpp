
#include "nanon/io/config.hpp"
#include "nanon/style/theme.hpp"

#include <QFont>

#include <iostream>
#include <sstream>
#include <stdexcept>


using namespace nanon::style;


NanonTheme::NanonTheme()
{

}


void NanonTheme::loadFromFile(QString fileName)
{
    nanon::io::ConfigParseError err;

    nanon::io::ConfigParser themeParser = nanon::io::ConfigParser();
    QVariant themeData = themeParser.parse(fileName, err);
    if (err.error != nanon::io::ConfigParseError::ParseError::NoError) {
        std::cout << "ERROR Could not load theme: " << qUtf8Printable(err.errorString) << std::endl;
        return;
    }

    QMap<QString, QVariant> rawTheme = themeData.toMap();

    loadFromData(rawTheme);
}

void NanonTheme::loadFromData(QMap<QString, QVariant> rawTheme)
{
    m_tokenColors.clear();

    if (rawTheme.contains("name")) {
        m_name = rawTheme["name"].toString();
    }

    if (rawTheme.contains("tokenColors")) {
        QList<QVariant> tokenColors = rawTheme["tokenColors"].toList();
        for (auto &entry : tokenColors) {
            QMap<QString, QVariant> tokenEntry = entry.toMap();
            TokenColor tokenColor;

            if (tokenEntry.contains("name")) {
                tokenColor.name = tokenEntry["name"].toString();
            }
            if (tokenEntry.contains("scope")) {
                // "scope" entry can be string or list of string
                if (tokenEntry["scope"].canConvert<QString>()) {
                    tokenColor.scopes.push_back(tokenEntry["scope"].toString());
                } else {
                    QList<QVariant> scopes = tokenEntry["scope"].toList();
                    for (auto &scope : scopes) {
                        tokenColor.scopes.push_back(scope.toString());
                    }
                }
            }
            if (tokenEntry.contains("settings")) {
                QMap<QString, QVariant> settings = tokenEntry["settings"].toMap();
                QTextCharFormat format;
                if (settings.contains("foreground")) {
                    QString foreground = settings["foreground"].toString();
                    format.setForeground(QColor(foreground));
                }
                if (settings.contains("fontStyle")) {
                    QString fontStyle = settings["fontStyle"].toString();
                    if (fontStyle == "italic") {
                        format.setFontItalic(true);
                    } else if (fontStyle == "bold") {
                        format.setFontWeight(QFont::Bold);
                    } else {
                        std::cout << "WARNING Unrecognised font style: " << qUtf8Printable(fontStyle) << std::endl;
                    }
                }
                tokenColor.format = format;
            }
            m_tokenColors.push_back(tokenColor);
        }
    }

    if (rawTheme.contains("colors")) {
        QMap<QString, QVariant> rawColors = rawTheme["colors"].toMap();
        for (const auto &[key, rawColor] : rawColors.asKeyValueRange()) {
            QColor color = QColor(rawColor.toString());
            m_colors[key] = color;
        }
    }
}



QColor NanonTheme::getColor(QString key)
{
    if (!m_colors.contains(key)) {
        std::stringstream err;
        err << "No such key '" << key.toStdString() << "'";
        throw std::runtime_error(err.str());
    }

    return m_colors[key];
}
