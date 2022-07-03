#ifndef NANON_UTILS_INCLUDED
#define NANON_UTILS_INCLUDED

#include <optional>
 #include <string>

#include <QJsonDocument>

std::optional<QJsonDocument> parseJsonFile(QString fileName);

#endif
