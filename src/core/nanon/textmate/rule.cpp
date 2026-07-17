
#include <iostream>

#include <QtCore/QVariant>

#include "nanon/textmate/rule.hpp"


using namespace nanon::textmate;


Rule::Rule(QString name)
{
    this->name = name;
}

Rule::~Rule()
{}


MatchRule::MatchRule(QString name, QString pattern)
: Rule(name), regex(pattern) {}


BeginEndRule::BeginEndRule(QString name, QString begin, QString end)
: Rule(name), begin(begin), end(end) {}


IncludeRule::IncludeRule(QString name, QString include)
: Rule(name), include(include) {}
