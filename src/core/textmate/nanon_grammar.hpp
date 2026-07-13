#pragma once

#include "nanon_rule.hpp"

#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QVariant>

#include <unordered_map>
#include <vector>


class Grammar : public RuleGroup
{
public:
    Grammar(QString scopeName, QMap<QString, QVariant> rawGrammar);
    virtual ~Grammar();

    RuleGroup root;

    QString scopeName;

    // List of all rules, including all rule group child rules,
    // which reference the rules stored here via ptr.
    std::vector<std::unique_ptr<Rule>> rules;

    std::unordered_map<QString, RuleGroup> repository;

    void ingestGrammar(QMap<QString, QVariant> rawGrammar);

private:
    void _ingestGrammar(QMap<QString, QVariant> rawGrammar);
    void _parsePatternArray(const QMap<QString, QVariant>& rawRule, RuleGroup& currentGroup);
    Rule* _parseRule(const QMap<QString, QVariant>& rawRule);

    RuleGroup* _resolveInclude(const QString& include);
};
