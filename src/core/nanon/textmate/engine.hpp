#pragma once

#include "nanon/textmate/grammar.hpp"
#include "nanon/textmate/rule.hpp"

#include <QtCore/QString>
#include <QtCore/QRegularExpressionMatch>

#include <unordered_set>


namespace nanon {
namespace textmate {


/**
 * Context represents a single begin/end rule or grammar rule group.
 */
struct Context
{
    /** The group of rules to apply for this context */
    RuleGroup* group;

    /** The begin/end rule that is active for this context */
    BeginEndRule* beginEndRule = nullptr;

    /** The match result for the begining of this context */
    QRegularExpressionMatch beginMatch;

    /** The compiled end regex with substituted captures from the begin match */
    QRegularExpression endRegex;
};


/**
 * Region represents the bounds of a successful match for a single textmate token (scope).
 */
struct Region {
    /** The textmate token/scope name */
    QString scope;

    /** The start character number */
    long start;

    /** The length of the region (starting from ``start``) */
    long length;
};


struct BlockState
{
    QVector<textmate::Context> stack;
    QVector<textmate::Region> regions;
};



class TextMateEngine
{
public:
    TextMateEngine();
    virtual ~TextMateEngine() = default;

    void setGrammar(std::unique_ptr<Grammar> grammar);
    void setGrammarFromFile(QString fileName);

    QVector<Region> parseBlock(int blockNumber, const QString& inputText);

    QVector<QString> scopesAtPosition(int blockNumber, int pos);

private:
    QVector<Context> m_stack;
    QHash<int, BlockState> m_blockCache;

    QVector<Region> parseLine(const QString& inputText);

    bool applyRule(
        Rule* rule,
        const QString& text,
        int& pos,
        QVector<Region>& regions,
        std::unordered_set<const RuleGroup*> &visited);

    std::unique_ptr<Grammar> m_grammar;
};

};  // namespace textmate
};  // namespace nanon
