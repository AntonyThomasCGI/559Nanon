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
    Grammar(QString scopeName, QMap<QString, QVariant> rawRule);
    virtual ~Grammar();

    RuleGroup root;

    QString scopeName;

    std::vector<std::unique_ptr<Rule>> rules;
    std::unordered_map<QString, RuleGroup> repository;

    void ingestGrammar(QMap<QString, QVariant> rawRule);

private:
    void _ingestGrammar(QMap<QString, QVariant> rawRule);
    void _parsePatternArray(const QMap<QString, QVariant>& raw, RuleGroup& currentGroup);
    Rule* _parseRule(const QMap<QString, QVariant>& raw, RuleGroup& currentGroup);

    RuleGroup* _resolveInclude(const QString& include);
};
