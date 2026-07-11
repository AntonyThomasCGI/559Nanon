#pragma once

#include "nanon_grammar.hpp"
#include "nanon_rule.hpp"

#include <QtCore/QString>
#include <QtCore/QRegularExpressionMatch>

#include <vector>


/**
 * Context represents a single begin/end rule or grammar rule group.
 */
struct Context
{
    // The group of rules to apply for this context
    RuleGroup* group;

    // The begin/end rule that is active for this context
    BeginEndRule* beginEndRule = nullptr;

    // The match result for the begining of this context
    QRegularExpressionMatch beginMatch;

    // The compiled end regex with substituted captures from the begin match
    QRegularExpression endRegex;
};


/**
 * Region represents the bounds of a successful match.
 */
struct Region {
    QString scope;
    long start;
    long length;
};


class TextMateEngine
{
public:
    TextMateEngine(RuleGroup* root);
    virtual ~TextMateEngine() = default;

    std::vector<Region> scanLine(const QString& inputText);
    bool applyRule(
        Rule* rule,
        const QString& text,
        int& pos,
        std::vector<Region>& regions,
        std::unordered_set<const RuleGroup*> &visited);

    std::vector<Context> stack;
};
