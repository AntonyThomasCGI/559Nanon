#pragma once


#include <QtCore/QRegularExpression>


class Pattern : public QRegularExpression
{
    Q_OBJECT

public:

    Pattern(QString pattern);
    virtual ~Pattern();


    // enum MatchType {match, begin, end, include};
    // MatchType matchType;
};

