
#include "nanon_grammar.hpp"

#include <iostream>


Grammar::Grammar(QString scopeName, QMap<QString, QVariant> rawRule)
{
    this->scopeName = scopeName;

    ingestGrammar(rawRule);
}


Grammar::~Grammar()
{}

void Grammar::ingestGrammar(QMap<QString, QVariant> rawRule)
{
    _ingestGrammar(rawRule);
}


void Grammar::_ingestGrammar(QMap<QString, QVariant> rawRule)
{
    // Populate repository
    if (rawRule.contains("repository"))
    {
        auto repo = rawRule["repository"].toMap();

        for (auto it = repo.begin(); it != repo.end(); ++it)
        {
            RuleGroup& group = repository[it.key()];

            _parsePatternArray(it.value().toMap(), group);
        }
    }

    // Resolve the list of patterns
    _parsePatternArray(rawRule, root);

    // Resolve includes
    for (auto& rule : rules) {
        if (auto include = dynamic_cast<IncludeRule*>(rule.get())) {
            include->resolved = _resolveInclude(include->include);
        }
    }
}

void Grammar::_parsePatternArray(const QMap<QString, QVariant>& object, RuleGroup& currentGroup)
{
    if (!object.contains("patterns")) {
        return;
    }

    auto list = object["patterns"].toList();

    for (const QVariant& value : list)
    {
        Rule* rule = _parseRule(value.toMap(), currentGroup);
        currentGroup.patterns.push_back(rule);
    }
}


Rule* Grammar::_parseRule(const QMap<QString, QVariant>& raw, RuleGroup& currentGroup)
{
    Rule* rule = nullptr;

    if (raw.contains("include"))
    {
        QString include = raw["include"].toString();
        auto ptr = std::make_unique<IncludeRule>(include, include);

        rule = ptr.get();
        rules.push_back(std::move(ptr));
    }

    if (raw.contains("match"))
    {
        auto ptr = std::make_unique<MatchRule>(
            raw["name"].toString(),
            raw["match"].toString());

        rule = ptr.get();
        rules.push_back(std::move(ptr));
    }
    else if (raw.contains("begin"))
    {
        auto ptr = std::make_unique<BeginEndRule>(
            raw["name"].toString(),
            raw["begin"].toString(),
            raw["end"].toString());

        for (auto it = raw.find("beginCaptures"); it != raw.end() && it.key() == "beginCaptures"; ++it)
        {
            // Parse begin captures
            auto captures = it.value().toMap();
            for (auto capIt = captures.begin(); capIt != captures.end(); ++capIt)
            {
                ptr->beginCaptures.push_back({
                    capIt.key().toInt(),
                    capIt.value().toMap()["name"].toString()
                });
            }
        }

        for (auto it = raw.find("endCaptures"); it != raw.end() && it.key() == "endCaptures"; ++it)
        {
            auto captures = it.value().toMap();
            for (auto capIt = captures.begin(); capIt != captures.end(); ++capIt)
            {
                ptr->endCaptures.push_back({
                    capIt.key().toInt(),
                    capIt.value().toMap()["name"].toString()
                });
            }
        }

        rule = ptr.get();
        rules.push_back(std::move(ptr));

        // Parse nested patterns belonging to this BeginEndRule.
        _parsePatternArray(raw,
            static_cast<BeginEndRule*>(rule)->children);
    }

    return rule;

}


RuleGroup* Grammar::_resolveInclude(const QString& include)
{
    if (include == "$self")
    {
        return &root;
    }
    else if (include == "$base")
    {
        return &root;
    }
    else if (include.startsWith("#"))
    {
        QString key = include.mid(1);

        auto it = repository.find(key);
        if (it != repository.end())
        {
            return &(it->second);
        }
        else
        {
            std::cout << "Warning: could not resolve include " << include.toStdString() << std::endl;
            return nullptr;
        }
    }

    std::cout << "Warning: could not resolve include " << include.toStdString() << std::endl;
    return nullptr;
}
