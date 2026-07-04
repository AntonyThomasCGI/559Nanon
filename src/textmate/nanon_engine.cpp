
#include "nanon_engine.hpp"

#include <iostream>


TextMateEngine::TextMateEngine(RuleGroup* root)
{
    stack.push_back({root, nullptr});
}


std::vector<Region> TextMateEngine::scanLine(const QString& text)
{

    std::vector<Region> regions;

    int pos = 0;

    while (pos < text.size())
    {
        Context& ctx = stack.back();

        // STEP 1: END RULE HAS PRIORITY
        if (ctx.activeRule)
        {
            auto match = ctx.activeRule->end.match(
                text,
                pos,
                QRegularExpression::NormalMatch,
                QRegularExpression::AnchorAtOffsetMatchOption
            );

            if (match.hasMatch())
            {
                regions.push_back({
                    ctx.activeRule->name,
                    pos,
                    match.capturedLength()
                });

                pos += match.capturedLength();
                stack.pop_back();
                continue;
            }
        }

        // STEP 2: TRY PATTERNS IN CURRENT GROUP
        bool matched = false;

        for (Rule* rule : ctx.group->patterns)
        {

            // MATCH RULE
            if (auto* m = dynamic_cast<MatchRule*>(rule))
            {
                auto match = m->regex.match(text, pos);

                if (match.hasMatch() && match.capturedStart() == 0)
                {
                    regions.push_back({
                        m->name,
                        pos,
                        match.capturedLength()
                    });

                    pos += match.capturedLength();
                    matched = true;
                    break;
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
                    regions.push_back({
                        b->name,
                        pos,
                        match.capturedLength()
                    });

                    pos += match.capturedLength();

                    // PUSH CONTEXT
                    stack.push_back({
                        &b->children,
                        b
                    });

                    for (Capture& cap : b->beginCaptures) {
                        int start = match.capturedStart(cap.group);
                        int length = match.capturedLength(cap.group);

                        if (start >= 0) {
                            regions.push_back({
                                cap.name,
                                start,
                                length
                            });
                        }
                    }

                    matched = true;
                    break;
                }
            }

        }

        // STEP 3: FALLBACK (consume 1 char)
        if (!matched)
        {
            // This is a simplistic way to handle the inherited scope
            if (ctx.activeRule) {
                regions.push_back({
                    ctx.activeRule->name,
                    pos,
                    1
                });
            }

            pos++;
        }
    }

    return regions;
}
