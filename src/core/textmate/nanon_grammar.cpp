
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

            Rule *rule = _parseRule(rawRule);
            if (rule != nullptr) {
                group.patterns.push_back(rule);
            } else {
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
}

void Grammar::_parsePatternArray(const QMap<QString, QVariant>& rawRule, RuleGroup& currentGroup)
{
    if (!rawRule.contains("patterns")) {
        return;
    }

    auto patternList = rawRule["patterns"].toList();

    for (const QVariant& pattern : patternList)
    {
        Rule* rule = _parseRule(pattern.toMap());
        currentGroup.patterns.push_back(rule);
    }
}


Rule* Grammar::_parseRule(const QMap<QString, QVariant>& rawRule)
{
    Rule* rule = nullptr;

    if (rawRule.contains("include"))
    {
        QString include = rawRule["include"].toString();
        auto ptr = std::make_unique<IncludeRule>(include, include);

        rule = ptr.get();
        rules.push_back(std::move(ptr));
    }

    if (rawRule.contains("match"))
    {
        auto ptr = std::make_unique<MatchRule>(
            rawRule["name"].toString(),
            rawRule["match"].toString());

        if (rawRule.contains("captures")) {
            QMap<QString, QVariant> captures = rawRule["captures"].toMap();
            for (auto capIt = captures.begin(); capIt != captures.end(); ++ capIt) {
                ptr->captures.push_back({
                    capIt.key().toInt(),
                    capIt.value().toMap()["name"].toString()
                });
            }
        }

        rule = ptr.get();
        rules.push_back(std::move(ptr));
    }
    else if (rawRule.contains("begin"))
    {
        QString name = rawRule.contains("name") ? rawRule["name"].toString() : "";
        auto ptr = std::make_unique<BeginEndRule>(
            name,
            rawRule["begin"].toString(),
            rawRule["end"].toString());

        if (rawRule.contains("beginCaptures"))
        {
            // Parse begin captures
            QMap<QString, QVariant> captures = rawRule["beginCaptures"].toMap();
            for (auto capIt = captures.begin(); capIt != captures.end(); ++capIt)
            {
                ptr->beginCaptures.push_back({
                    capIt.key().toInt(),
                    capIt.value().toMap()["name"].toString()
                });
            }
        }

        if (rawRule.contains("endCaptures"))
        {
            QMap<QString, QVariant> captures = rawRule["endCaptures"].toMap();
            for (auto capIt = captures.begin(); capIt != captures.end(); ++capIt)
            {
                ptr->endCaptures.push_back({
                    capIt.key().toInt(),
                    capIt.value().toMap()["name"].toString()
                });
            }
        }

        // Parse nested patterns belonging to this BeginEndRule.
        _parsePatternArray(rawRule, ptr->children);

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
