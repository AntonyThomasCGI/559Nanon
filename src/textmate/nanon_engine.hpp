#pragma once

#include "nanon_grammar.hpp"
#include "nanon_rule.hpp"

#include <QtCore/QString>

#include <vector>


struct Context
{
    RuleGroup* group;
    BeginEndRule* activeRule = nullptr;
};


class TextMateEngine
{
public:
    TextMateEngine(RuleGroup* root);
    virtual ~TextMateEngine() = default;

    std::vector<Region> scanLine(const QString& text);

    std::vector<Context> stack;
};
