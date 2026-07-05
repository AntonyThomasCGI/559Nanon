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


class MatchRule : public Rule
{
public:
    MatchRule(QString name, QString pattern);

    QRegularExpression regex;
};


struct Capture {
    int group;
    QString name;
};


class BeginEndRule : public Rule
{
public:


    BeginEndRule(QString name, QString begin, QString end);

    QRegularExpression begin;
    QRegularExpression end;

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
