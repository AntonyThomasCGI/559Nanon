#pragma once

#include <QtCore/QString>
#include <QtCore/QRegularExpression>
#include <QtCore/QList>

#include <memory>
#include <tuple>
#include <vector>



class Rule
{
public:
    Rule(QString name);
    virtual ~Rule();

    QString name;

    std::vector<std::unique_ptr<Rule>> patterns;

};



class RuleGroup
{
public:
    RuleGroup() = default;
    virtual ~RuleGroup() = default;

    std::vector<Rule*> patterns;
};


// TODO, captures can have patterns. But this seems obscurely
// used in most grammar files I've looked at
struct Capture {
    int group;
    QString name;
};


class MatchRule : public Rule
{
public:
    MatchRule(QString name, QString pattern);

    std::vector<Capture> captures;

    QRegularExpression regex;
};


class BeginEndRule : public Rule
{
public:


    BeginEndRule(QString name, QString begin, QString end);

    QRegularExpression begin;
    QString end;

    std::vector<Capture> beginCaptures;
    std::vector<Capture> endCaptures;

    RuleGroup children;
};


class IncludeRule : public Rule
{
public:
    IncludeRule(QString name, QString include);

    QString include;
    RuleGroup* resolved = nullptr;
};
