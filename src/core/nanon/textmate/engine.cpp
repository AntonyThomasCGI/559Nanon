
#include "nanon/io/config.hpp"
#include "nanon/textmate/engine.hpp"

#include <algorithm>
#include <iostream>


using namespace nanon::textmate;


TextMateEngine::TextMateEngine()
{
}


void TextMateEngine::setGrammar(std::unique_ptr<Grammar> grammar)
{
    m_stack.clear();
    m_blockCache.clear();

    m_grammar = std::move(grammar);
    m_stack.push_back({&m_grammar->root});
}


void TextMateEngine::setGrammarFromFile(QString fileName)
{
    nanon::io::ConfigParseError err;

    nanon::io::ConfigParser tmParser = nanon::io::ConfigParser();
    QVariant tmData = tmParser.parse(fileName, err);
    if (err.error != nanon::io::ConfigParseError::ParseError::NoError) {
        std::cout << "ERROR Could not load textmate grammar: " << qUtf8Printable(err.errorString) << std::endl;
        return;
    }

    QMap<QString, QVariant> tmMap = tmData.toMap();

    auto grammar = std::make_unique<textmate::Grammar>("my scope", tmMap);
    setGrammar(std::move(grammar));
}


void pushRegion(QString &name, int start, int length, QVector<Region> &regions)
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


QVector<Region> TextMateEngine::parseBlock(int blockNumber, const QString& inputText)
{
    if (!m_grammar) {
        std::cout << "WARNING: No grammar set but parseBlock called" << std::endl;
        return {};
    }

    if (m_blockCache.contains(blockNumber - 1)) {
        m_stack = m_blockCache[blockNumber - 1].stack;
    } else {
        m_stack.clear();
        m_stack.push_back({&m_grammar->root, nullptr});
    }

    QVector<Region> regions = parseLine(inputText);

    BlockState state;
    state.stack = m_stack;
    state.regions = regions;

    m_blockCache[blockNumber] = state;

    return regions;
}


QVector<QString> TextMateEngine::scopesAtPosition(QTextBlock block, int pos)
{
    int blockNumber = block.blockNumber();
    if (!m_blockCache.contains(blockNumber)) {
        std::cout << "ERROR: Current block has no cached scopes!" << std::endl;
        return {};
    }

    // The cursor can be placed one position further than the current text length,
    // in this case the scope should be determined via the last position.
    int lastPosition = block.text().length() - 1;
    int targetPosition = std::min(pos, lastPosition);

    BlockState blockState = m_blockCache[blockNumber];
    QVector<QString> scopes;
    for (const textmate::Region& r : blockState.regions)
    {
        if (targetPosition >= r.start && targetPosition < r.start + r.length)
        {
            scopes.push_back(r.scope);
        }
    }
    return scopes;
}


QVector<Region> TextMateEngine::parseLine(const QString& inputText)
{

    // TODO, idk what the interaction is with "$" matches now
    const QString text = inputText + "\n";

    QVector<Region> regions;

    int pos = 0;

    while (pos <= text.size())
    {
        Context& ctx = m_stack.back();

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
                m_stack.pop_back();
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
    for (auto &ctx : m_stack) {
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
    QVector<Region>& regions,
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
            m_stack.push_back(ctx);

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
