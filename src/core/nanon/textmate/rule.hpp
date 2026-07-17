#pragma once

#include <QtCore/QString>
#include <QtCore/QRegularExpression>
#include <QtCore/QList>

#include <memory>
#include <tuple>
#include <vector>


namespace nanon {
namespace textmate {


/**
 * The base class for all textmate rules.
 */
class Rule
{
public:
    Rule(QString name);
    virtual ~Rule();

    QString name;

    std::vector<std::unique_ptr<Rule>> patterns;

};



/**
 * A collection of rules.
 */
class RuleGroup
{
public:
    RuleGroup() = default;
    virtual ~RuleGroup() = default;

    std::vector<Rule*> patterns;
};


/**
 * A regex capture group and scope name.
 */
struct Capture {
    /** The regex group to this capture is for */
    int group;

    /** The textmate scope name */
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


};  // namespace textmate
};  // namespace nanon
