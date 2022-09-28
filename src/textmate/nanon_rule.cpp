
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
: Rule(name), match(pattern) {}


std::tuple<State, Regions> MatchRule::search(const QString &text)
{
    Regions regions;

    QRegularExpressionMatchIterator matchIterator = match.globalMatch(text);
    while (matchIterator.hasNext()) {
        QRegularExpressionMatch match = matchIterator.next();

        Region region = {name, match.capturedStart(), match.capturedEnd()};
        regions.push_back(region);
    }

    return std::make_tuple(State{}, regions);
}


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


Grammar::Grammar(QString scopeName, QMap<QString, QVariant> rawRule)
{
    this->scopeName = scopeName;

    makeRules(rawRule);
}


Grammar::~Grammar()
{}

void Grammar::makeRules(QMap<QString, QVariant> rawRule)
{

    if (rawRule.contains("match") && rawRule.contains("name")) {
        QString name = rawRule.value("name").toString();
        QString matchPattern = rawRule.value("match").toString();

        rules.push_back( std::unique_ptr<Rule>(new MatchRule(name, matchPattern)) );
    }


    if (rawRule.contains("begin") && rawRule.contains("end")) {
        QString name = rawRule.value("name").toString();
        QString begin = rawRule.value("begin").toString();
        QString end = rawRule.value("end").toString();

        rules.push_back( std::unique_ptr<Rule>(new BeginEndRule(name, begin, end)));
    }
    // if (map.contains("end"))
    //     rule.end = QRegularExpression(map.value("end").toString());
    // if (map.contains("while"))
    //     rule.while_ = QRegularExpression(map.value("while").toString());
    // if (map.contains("include"))
    //     rule.include = map.value("include").toString();
    // if (map.contains("contentName"))
    //     rule.contentName = map.value("contentName").toString();

    // This won't work when pattern starts depending on parent rules.
    // This just blindly adds everything as it's own rule.
    if (rawRule.contains("patterns")) {
        QList<QVariant> allPatterns = rawRule.value("patterns").toList();
        for (int i = 0; i < allPatterns.size(); ++i) {
            QMap<QString, QVariant> patternMap = allPatterns.at(i).toMap();

            makeRules(patternMap);
        }
    }

    // if (map.contains("captures")) {
    //     QMap<QString, QVariant> capturesMap = map.value("captures").toMap();
    //     QMapIterator<QString, QVariant> it(capturesMap);
    //     while (it.hasNext()) {
    //         it.next();
    //         QMap<QString, QVariant> m = it.value().toMap();
    //         rule.captures.insert(it.key().toInt(), makeRule(m, blockStateID));
    //     }
    // }

    // if (map.contains("beginCaptures")) {
    //     QMap<QString, QVariant> beginCapturesMap = map.value("beginCaptures").toMap();
    //     QMapIterator<QString, QVariant> it(beginCapturesMap);
    //     while (it.hasNext()) {
    //         it.next();
    //         QMap<QString, QVariant> m = it.value().toMap();
    //         rule.beginCaptures.insert(it.key().toInt(), makeRule(m, blockStateID));
    //     }
    // }

    // if (map.contains("endCaptures")) {
    //     QMap<QString, QVariant> endCapturesMap = map.value("endCaptures").toMap();
    //     QMapIterator<QString, QVariant> it(endCapturesMap);
    //     while (it.hasNext()) {
    //         it.next();
    //         QMap<QString, QVariant> m = it.value().toMap();
    //         rule.endCaptures.insert(it.key().toInt(), makeRule(m, blockStateID));
    //     }
    // }

    // if (map.contains("whileCaptures")) {
    //     QMap<QString, QVariant> whileCapturesMap = map.value("whileCaptures").toMap();
    //     QMapIterator<QString, QVariant> it(whileCapturesMap);
    //     while (it.hasNext()) {
    //         it.next();
    //         QMap<QString, QVariant> m = it.value().toMap();
    //         rule.whileCaptures.insert(it.key().toInt(), makeRule(m, blockStateID));
    //     }
    // }

    // return rule;
}