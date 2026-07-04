
#include <iostream>

#include <QtCore/QVariant>

#include "textmate/nanon_rule.hpp"


Rule::Rule(QString name)
{
    this->name = name;
}

Rule::~Rule()
{}

// QList<QRegularExpression> Rule::getPatterns()
// {
//     perror("not implemented");

//     QList<QRegularExpression> ls;

//     return ls;
// }


MatchRule::MatchRule(QString name, QString pattern)
: Rule(name), regex(pattern) {}


BeginEndRule::BeginEndRule(QString name, QString begin, QString end)
: Rule(name), begin(begin), end(end) {}



// todo: maybe need two functions: start / search so that we can easily pass state
// back into the 'search' to try match only end. then start only becomes responsible
// for matching first index, search tries to return end. Must somehow have to stop
// search when it comes up in normal grammar>rules iteration though.
std::tuple<State, Regions> BeginEndRule::search(const QString &text)
{
    Regions regions;

    int startIndex = text.indexOf(begin, 0);
    while (startIndex >= 0) {
        QRegularExpressionMatch match = end.match(text, startIndex + 1);
        int endIndex = match.capturedStart();
        int capturedLength = 0;
        if (endIndex == -1) {  // End is not in this block.
            capturedLength = text.length() - startIndex;

        } else {
            capturedLength = endIndex - startIndex + match.capturedLength();
        }
        Region region = {name, startIndex, endIndex};
        regions.push_back(region);

        startIndex = text.indexOf(begin, startIndex + capturedLength);
    }

    return std::make_tuple(State{}, regions);
}

