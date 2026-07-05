#pragma once

#include "nanon_grammar.hpp"
#include "nanon_rule.hpp"

#include <QtCore/QString>

#include <vector>


/**
 * Context represents the current state of the engine as it scans through a line of text.
 */
struct Context
{
    RuleGroup* group;
    BeginEndRule* activeRule = nullptr;
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

    std::vector<Region> scanLine(const QString& text);
    bool applyRule(Rule* rule, const QString& text, int& pos, std::vector<Region>& regions);

    std::vector<Context> stack;
};
