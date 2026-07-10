
#include "nanon_engine.hpp"

#include <iostream>


TextMateEngine::TextMateEngine(RuleGroup* root)
{
    stack.push_back({root, nullptr, -1});
}


std::vector<Region> TextMateEngine::scanLine(const QString& text)
{

    std::vector<Region> regions;

    int pos = 0;

    std::cout << "Test size is " << std::to_string(text.size()) << std::endl;

    while (pos <= text.size())
    {
        Context& ctx = stack.back();

        // STEP 1: END RULE HAS PRIORITY
        if (ctx.activeRule != nullptr)
        {
            //auto match = ctx.activeRule->end.match(
            //    text,
            //    pos,
            //    QRegularExpression::NormalMatch
            //    //QRegularExpression::AnchorAtOffsetMatchOption
            //);

            std::cout << "trying to match: '" << text.toStdString() << " at pos " << std::to_string(pos) << std::endl;

            auto match = ctx.activeRule->end.match(text, pos);

            if (match.hasMatch() && match.capturedStart() == pos)
            {
                for (Capture& cap : ctx.activeRule->endCaptures) {
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

                if (!ctx.activeRule->name.isEmpty()) {

                    regions.push_back({
                        ctx.activeRule->name,
                        ctx.beginPosition,
                        pos + match.capturedLength() - ctx.beginPosition
                    });
                }

                //pos += match.capturedLength();
                stack.pop_back();
                continue;
            }
        }

        bool matched = false;

        // STEP 2: TRY PATTERNS IN CURRENT GROUP
        if (ctx.group != nullptr) {
            for (Rule* rule : ctx.group->patterns)
            {
                // INCLUDE RULE
                if (auto* i = dynamic_cast<IncludeRule*>(rule))
                {
                    if (i->resolved)
                    {
                        for (Rule* r : i->resolved->patterns)
                        {
                            if (applyRule(r, text, pos, regions)) {
                                matched = true;
                                break;
                            }
                        }
                    }
                } else {
                    matched = applyRule(rule, text, pos, regions);
                }
            }
        }

        // Add region for each active rule in stack
        //for (auto &item : stack) {
        //    if (item.activeRule != nullptr &&
        //        item.beginPosition != -1 &&
        //        !item.activeRule->name.isEmpty())
        //    {
        //            std::cout << "add scope " << item.activeRule->name.toStdString() << std::endl;
        //            regions.push_back({
        //                item.activeRule->name,
        //                ctx.beginPosition,
        //                pos
        //            });
        //    }
        //}

        // STEP 3: FALLBACK (consume 1 char)
        if (!matched)
        {
            pos++;
        }
    }

    return regions;
}


bool TextMateEngine::applyRule(Rule* rule, const QString& text, int& pos, std::vector<Region>& regions)
{
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
            regions.push_back({
                m->name,
                pos,
                match.capturedLength()
            });

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
            std::cout << "matched begin: ";
            // Not all begin/end rules have a name key. If they do though, push a region.
            //if (!b->name.isEmpty()) {
            //    regions.push_back({
            //        b->name,
            //        pos,
            //        match.capturedLength()
            //    });
            //    std::cout << b->name.toStdString();
            //}
            //std::cout << " at pos: " << std::to_string(pos) << std::endl;

            // PUSH CONTEXT
            stack.push_back({
                &b->children,
                b,
                pos
            });

            pos += match.capturedLength();

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

            return true;
        }
    }

    return false;
}
