
#include "nanon_engine.hpp"

#include <iostream>


TextMateEngine::TextMateEngine(RuleGroup* root)
{
    stack.push_back({root});
}


void pushRegion(QString &name, int start, int length, std::vector<Region> &regions)
{
    if (name.isEmpty()) {
        return;
    }
    QStringList tokens = name.split(" ");
    for (auto it = tokens.crbegin(); it < tokens.crend(); ++it) {
        regions.push_back({
            *it,
            start,
            length
        });
    }
}


std::vector<Region> TextMateEngine::scanLine(const QString& inputText)
{

    // TODO, idk what the interaction is with "$" matches now
    const QString text = inputText + "\n";

    std::vector<Region> regions;

    int pos = 0;

    while (pos <= text.size())
    {
        Context& ctx = stack.back();

        // STEP 1: END RULE HAS PRIORITY
        if (ctx.beginEndRule != nullptr)
        {
            auto match = ctx.endRegex.match(text, pos);

            if (match.hasMatch() && match.capturedStart() == pos)
            {
                for (Capture& cap : ctx.beginEndRule->endCaptures) {
                    int start = match.capturedStart(cap.group);
                    int length = match.capturedLength(cap.group);

                    if (start >= 0) {
                        pushRegion(cap.name, start, length, regions);
                    }
                }

                pushRegion(ctx.beginEndRule->name,
                           ctx.beginMatch.capturedStart(),
                           pos + match.capturedLength() - ctx.beginMatch.capturedStart(),
                           regions);

                pos += match.capturedLength();
                stack.pop_back();
                continue;
            }
        }

        bool matched = false;

        // STEP 2: TRY PATTERNS IN CURRENT GROUP
        if (ctx.group != nullptr) {
            for (Rule* rule : ctx.group->patterns)
            {
                std::unordered_set<const RuleGroup*> visited;
                if (applyRule(rule, text, pos, regions, visited)) {
                    matched = true;
                    break;
                }
            }
        }

        // STEP 3: FALLBACK (consume 1 char)
        if (!matched)
        {
            pos++;
        }
    }

    // Emit regions for any contexts that are still active
    for (auto &ctx : stack) {
        if (ctx.beginEndRule) {

            pushRegion(ctx.beginEndRule->name,
                       ctx.beginMatch.capturedStart(),
                       pos,
                       regions);
        }
    }

    return regions;
}


bool TextMateEngine::applyRule(
    Rule* rule,
    const QString& text,
    int& pos,
    std::vector<Region>& regions,
    std::unordered_set<const RuleGroup*> &visited)
{
    // INCLUDE RULE
    if (auto* i = dynamic_cast<IncludeRule*>(rule))
    {
        if (!i->resolved) {
            return false;
        }

        // Already expanded this RuleGroup?
        if (!visited.insert(i->resolved).second) {
            return false;
        }

        for (Rule* child : i->resolved->patterns)
        {
            if (applyRule(child, text, pos, regions, visited)) {
                visited.erase(i->resolved);
                return true;
            }
        }
    }

    // MATCH RULE
    if (auto* m = dynamic_cast<MatchRule*>(rule))
    {
        auto match = m->regex.match(
            text,
            pos,
            QRegularExpression::NormalMatch,
            QRegularExpression::AnchorAtOffsetMatchOption
        );

        if (match.hasMatch())
        {
            // Apply match region
            pushRegion(m->name,
                       pos,
                       match.capturedLength(),
                       regions);

            // Apply match captures
            for (Capture& cap : m->captures) {
                int start = match.capturedStart(cap.group);
                int length = match.capturedLength(cap.group);

                if (start >= 0) {
                    pushRegion(cap.name,
                               start,
                               length,
                               regions);
                }
            }

            pos += match.capturedLength();

            return true;
        }
    }

    // BEGIN/END RULE
    else if (auto* b = dynamic_cast<BeginEndRule*>(rule))
    {

        auto match = b->begin.match(
            text,
            pos,
            QRegularExpression::NormalMatch,
            QRegularExpression::AnchorAtOffsetMatchOption
        );

        if (match.hasMatch())
        {
            // Construct the end regex by substituting capture groups
            QString endRegex = b->end;
            for (int i = 0; i <= match.lastCapturedIndex(); ++i) {
                endRegex.replace(
                    "\\" + QString::number(i),
                    QRegularExpression::escape(match.captured(i))
                );
            }

            // Push the new context
            Context ctx;
            ctx.group = &b->children;
            ctx.beginEndRule = b;
            ctx.beginMatch = match;
            ctx.endRegex = QRegularExpression(endRegex);
            stack.push_back(ctx);

            pos += match.capturedLength();

            for (Capture& cap : b->beginCaptures) {
                int start = match.capturedStart(cap.group);
                int length = match.capturedLength(cap.group);

                if (start >= 0) {
                    pushRegion(cap.name,
                               start,
                               length,
                               regions);
                }
            }

            return true;
        }
    }

    return false;
}
