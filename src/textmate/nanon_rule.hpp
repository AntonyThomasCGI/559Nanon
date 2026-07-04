#pragma once

#include <QtCore/QString>
#include <QtCore/QRegularExpression>
#include <QtCore/QList>

#include <memory>
#include <tuple>
#include <vector>


// Forward declaration.
class Rule;


class RuleContainer
{
public:
    RuleContainer() = default;
    virtual ~RuleContainer() = default;

    std::vector<Rule*> patterns;

};


class RuleGroup : public RuleContainer
{
public:
    RuleGroup() = default;
    virtual ~RuleGroup() = default;
};



// Region represents the bounds of a successful match.
struct Region {
    QString scope;
    long start;
    long length;
};

// Forward declaration.
class Rule;

typedef std::vector<Region> Regions;
typedef std::vector<Rule*>  State;


class Rule
{
public:
    Rule(QString name);
    virtual ~Rule();

    QString name;
    // QString contentName;

    std::vector<std::unique_ptr<Rule>> patterns;

};

// class CaptureRule : public Rule
// {
//     // pass
// };

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

    std::tuple<State, Regions> search(const QString &text);

    QRegularExpression begin;
    QRegularExpression end;

    std::vector<Capture> beginCaptures;
    std::vector<Capture> endCaptures;

    RuleGroup children;
};


// class BeginWhileRule : public Rule
// {
//     // pass
// };


// struct RawRule
// {
//     int scopeID;
//     QString name;
//     QRegularExpression match;
//     QRegularExpression begin;
//     QRegularExpression end;
//     QRegularExpression while_;
//     QString include;
//     QString contentName;
//     QMap<int, Rule> captures;
//     QMap<int, Rule> beginCaptures;
//     QMap<int, Rule> endCaptures;
//     QMap<int, Rule> whileCaptures;
//     std::vector<Rule> patterns;
// };




// https://github.com/microsoft/vscode-textmate/blob/main/src/rule.ts
// public static getCompiledRuleId...

// if match:
    // return new MatchRule
// if !begin
    // include only rule, get repository, get patterns
// if while
    // return new begin/while rule
// return new begin/end rule

// so basically you can have:
// - 'match' rule
// - 'begin + while' rule
// - 'include' rule
// - 'begin + end' rule

// match -> captures
// begin -> beginCaptures
// end   -> endCaptures
// while -> whileCaptures

// gotcha: captures for begin/end rule is short-hand for a beginCaptures/endCaptures with same values



// for each line
    // for each rule
        // try to match rule.pattern[0]
        // when we match, should return the `type` of match - eg; isEndMatch = true;
        // if we found new `start` rule
            // push onto rule stack
        // if we found `end` rule
            // pop from rule stack
        // if we found `match` rule
            // don't touch rule stack

        // while also constructing rule stack, we should construct `region` list of match start/end regions
        // rule stack gets dumped onto region list after end of line reached

