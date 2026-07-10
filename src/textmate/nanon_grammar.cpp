
#include "nanon_grammar.hpp"

#include <iostream>


Grammar::Grammar(QString scopeName, QMap<QString, QVariant> rawGrammar)
{
    this->scopeName = scopeName;

    ingestGrammar(rawGrammar);
}


Grammar::~Grammar()
{}

void Grammar::ingestGrammar(QMap<QString, QVariant> rawGrammar)
{
    _ingestGrammar(rawGrammar);
}


void Grammar::_ingestGrammar(QMap<QString, QVariant> rawGrammar)
{
    // Populate repository
    if (rawGrammar.contains("repository"))
    {
        auto repo = rawGrammar["repository"].toMap();

        for (auto it = repo.begin(); it != repo.end(); ++it)
        {
            QMap<QString, QVariant> rawRule = it.value().toMap();
            RuleGroup& group = repository[it.key()];

            if (rawRule.contains("begin")) {
                Rule *rule = _parseRule(rawRule);
                group.patterns.push_back(rule);
            } else {
                // Assume pattern array
                _parsePatternArray(it.value().toMap(), group);
            }

        }
    }

    // Resolve the list of patterns
    _parsePatternArray(rawGrammar, root);

    // Resolve includes
    for (auto& rule : rules) {
        if (auto include = dynamic_cast<IncludeRule*>(rule.get())) {
            include->resolved = _resolveInclude(include->include);
        }
    }
    std::cout << "done" << std::endl;
}

void Grammar::_parsePatternArray(const QMap<QString, QVariant>& rawRule, RuleGroup& currentGroup)
{
    if (!rawRule.contains("patterns")) {
        return;
    }

    auto patternList = rawRule["patterns"].toList();

    for (const QVariant& value : patternList)
    {
        Rule* rule = _parseRule(value.toMap());
        currentGroup.patterns.push_back(rule);
    }
}


Rule* Grammar::_parseRule(const QMap<QString, QVariant>& raw)
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
        QString name = raw.contains("name") ? raw["name"].toString() : "";
        auto ptr = std::make_unique<BeginEndRule>(
            name,
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

        // Parse nested patterns belonging to this BeginEndRule.
        _parsePatternArray(raw, ptr->children);

        rule = ptr.get();
        rules.push_back(std::move(ptr));

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
