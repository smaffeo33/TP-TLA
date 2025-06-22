#include "Generator.h"
#include "../../frontend/syntactic-analysis/AbstractSyntaxTree.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

static Logger   *_logger     = NULL;
static const char _indentChar = ' ';
static const unsigned _indentSize = 4;
static unsigned _uniqueId    = 0;

static void   _printIndent (FILE *f, unsigned lvl);
static void   _out         (FILE *f, unsigned lvl, const char *fmt, ...);
static const char *_sanitize    (const char *name);
static const char *_aliasToState(AliasType a);
static void   _genProgramCss   (FILE *f, Program *p);
static void   _genTriggerCss   (FILE *f, Trigger *t);
static void   _genStateClassCss(FILE *f, const char *triggerName, State *s);
static void   _genStylePropsCss(FILE *f, unsigned lvl, PropertyList *plist);
static void   _genStepItem     (FILE *f, const char *prefix, StepItem *step);
static void   _genTransitionCss(FILE *f, Trigger *t, Transition *tr);
static void   _genAnimateCss   (FILE *f, const char *prefix, Animate *a,
                                Style *fromStyle, Style *toStyle);

void initializeGeneratorModule(void)  { _logger = createLogger("Generator"); }
void shutdownGeneratorModule(void)    { destroyLogger(_logger);            }

static const char *_normalizeSelectorName(const char *sel) {
    static char buf[128];
    size_t j = 0;
    for (size_t i = 0; sel && sel[i] && j < sizeof(buf) - 1; ++i) {
        if (sel[i] != '.' && sel[i] != '#') buf[j++] = sel[i];
    }
    buf[j] = '\0';
    return buf;
}

void generate(CompilerState *compilerState)
{
    if (!compilerState || !compilerState->abstractSyntaxtTree) {
        logError(_logger, "Nothing to generate (NULL AST).");
        return;
    }

    _uniqueId = 0;

    logDebugging(_logger, "Generating CSS…");
    _genProgramCss(stdout, compilerState->abstractSyntaxtTree);
}

static void _printIndent(FILE *f, unsigned lvl)
{
    for (unsigned i = 0; i < lvl * _indentSize; ++i)
        fputc(_indentChar, f);
}

static void _out(FILE *f, unsigned lvl, const char *fmt, ...)
{
    _printIndent(f, lvl);

    va_list ap; va_start(ap, fmt);
    vfprintf(f, fmt, ap);
    va_end(ap);

    size_t len = strlen(fmt);
    if (len == 0 || fmt[len - 1] != '\n')
        fputc('\n', f);
}

static const char *_sanitize(const char *name)
{
    if (!name)      return "";
    if (strcmp(name, "*")   == 0) return "active";
    if (strcmp(name, "void") == 0) return "void";
    return name;
}

static const char *_aliasToState(AliasType a)
{
    switch (a) {
        case ENTER:     return "enter";
        case LEAVE:     return "leave";
        case INCREMENT: return "increment";
        case DECREMENT: return "decrement";
        default:        return "error";
    }
}

static void _genStylePropsCss(FILE *f, unsigned lvl, PropertyList *plist)
{
    if (!plist) return;

    for (size_t i = 0; i < plist->propertyCount; ++i) {
        Property *p = (plist->properties) ? plist->properties[i] : NULL;
        if (!p || !p->name) continue;

        const char *name = p->name;

        char val[64] = {0};
        switch (p->type) {
            case UNITLESS:
                snprintf(val, sizeof val, "%g", p->floatValue);
                break;
            case UNIT:
            case STRING:
            case COLOR:
                if (p->value && *p->value)
                    strncpy(val, p->value, sizeof val - 1);
                else
                    snprintf(val, sizeof val, "%g", p->floatValue);
                break;
            default:
                break;
        }

        if (strcmp(name, "scale") == 0) {
            if (!*val) strcpy(val, "1");
            _out(f, lvl, "transform: scale(%s);", val);
            continue;
        }
        if (strcmp(name, "translateY") == 0 || strcmp(name, "translateX") == 0) {
            const char *func = (strcmp(name, "translateY") == 0) ? "translateY" : "translateX";
            _out(f, lvl, "transform: %s(%s);", func, val);
            continue;
        }

        if (*val) {
            _out(f, lvl, "%s: %s;", name, val);
        }
    }
}

static void _genStateClassCss(FILE *f, const char *triggerName, State *s)
{
    const char *san = _sanitize(s->name);
    if (strcmp(san, "void") == 0) return;

    _out(f, 0, ".%s-%s {", triggerName, san);
    _genStylePropsCss(f, 1, s->style ? s->style->properties : NULL);
    _out(f, 0, "}");
}

static void _genAnimateCss(FILE *f,
                           const char *prefix,
                           Animate    *a,
                           Style      *fromStyle,
                           Style      *toStyle)
{
    if (!a) return;

    AnimateInfo *ai = a->animateInfo;
    const char *dur = (ai && ai->duration) ? ai->duration : "0ms";
    const char *eas = (ai && ai->easing)   ? ai->easing   : "ease";
    const char *del = (ai && ai->delay)    ? ai->delay    : "0ms";
    int   hasDelay = (ai && ai->delay && strcmp(del, "0ms") != 0 && strcmp(del, "0") != 0);

    unsigned id = _uniqueId++;
    char kfName [128]; snprintf(kfName,  sizeof kfName,  "%s-kf-%u",   prefix, id);
    char clsName[256]; snprintf(clsName, sizeof clsName, "%s-anim-%u", prefix, id);

    PropertyList *destProps = NULL;
    if (toStyle) destProps = toStyle->properties;
    else if (a->type == ANIMATE_WITH_STYLE && a->style)
        destProps = a->style->properties;

    _out(f, 0, "@keyframes %s {", kfName);

    if (a->type == ANIMATE_WITH_KEYFRAMES && a->keyframes && a->keyframes->keyframeStyleList) {
        KeyframeStyleList *kl = a->keyframes->keyframeStyleList;
        for (size_t i = 0; i < kl->keyframeCount; ++i) {
            KeyframeStyle *ks = kl->keyframeStyles[i];
            float pct = (ks->offset <= 1.f) ? ks->offset * 100.f : ks->offset;
            _out(f, 1, "%.0f%% {", pct);
            _genStylePropsCss(f, 2, ks->properties);
            _out(f, 1, "}");
        }
    } else {
        _out(f, 1, "from {");
        if (fromStyle) {
            _genStylePropsCss(f, 2, fromStyle->properties);
        }
        _out(f, 1, "}");

        _out(f, 1, "to {");
        if (toStyle) {
            _genStylePropsCss(f, 2, toStyle->properties);
        } else if (a->type == ANIMATE_WITH_STYLE && a->style) {
            _genStylePropsCss(f, 2, a->style->properties);
        }
        _out(f, 1, "}");
    }

    _out(f, 0, "}");

    _out(f, 0, ".%s {", clsName);
    if (hasDelay)
        _out(f, 1, "animation: %s %s %s %s forwards;", kfName, dur, eas, del);
    else
        _out(f, 1, "animation: %s %s %s forwards;", kfName, dur, eas);
    _out(f, 0, "}");
}

static void _genStepItem(FILE *f, const char *prefix, StepItem *step)
{
    if (!step) return;

    switch (step->type) {
        case ANIMATE_ITEM:
            _genAnimateCss(f, prefix, (Animate *)step->item, NULL, NULL);
            break;

        case STYLE_ITEM: {
            Style *st = (Style *)step->item;
            char tmp[64];
            snprintf(tmp, sizeof tmp, "%s-style-%u", prefix, _uniqueId++);
            _out(f, 0, ".%s {", tmp);
            _genStylePropsCss(f, 1, st ? st->properties : NULL);
            _out(f, 0, "}");
            break;
        }

        case GROUP_ITEM:
        case SEQUENCE_ITEM: {
            StepItemList *lst = (step->type == GROUP_ITEM)
                                ? ((Group *)step->item)->stepItemList
                                : ((Sequence *)step->item)->stepItemList;
            for (size_t i = 0; i < lst->itemCount; ++i)
                _genStepItem(f, prefix, lst->items[i]);
            break;
        }

        case QUERY_ITEM: {
            Query *q = (Query *)step->item;
            const char *sel = (q->selectorType == ALIAS_SELECTOR)
                              ? _sanitize(_aliasToState(q->alias))
                              : _normalizeSelectorName(q->selector);

            char queryPrefix[256];
            snprintf(queryPrefix, sizeof queryPrefix, "%s-query-%s", prefix, sel);

            for (size_t i = 0; i < q->stepItemList->itemCount; ++i)
                _genStepItem(f, queryPrefix, q->stepItemList->items[i]);
            break;
        }

        case ANIMATE_CHILD_ITEM: {
            static char lastChildPrefix[256] = {0};
            if (strcmp(lastChildPrefix, prefix) != 0) {
                strcpy(lastChildPrefix, prefix);

                char childPrefix[256];
                snprintf(childPrefix, sizeof childPrefix, "%s-child-anim", prefix);

                _out(f, 0, "@keyframes %s-kf {", childPrefix);
                _out(f, 1, "from {");
                _out(f, 2, "opacity: 0;");
                _out(f, 2, "transform: translateY(10px);");
                _out(f, 1, "}");
                _out(f, 1, "to {");
                _out(f, 2, "opacity: 1;");
                _out(f, 2, "transform: translateY(0);");
                _out(f, 1, "}");
                _out(f, 0, "}");

                _out(f, 0, ".%s > * {", prefix);
                _out(f, 1, "animation: %s-kf 0.3s ease-out forwards;", childPrefix);
                _out(f, 0, "}");
            }
            break;
        }

        case STAGGER_ITEM: {
            Stagger *stag = (Stagger *)step->item;
            if (!stag) break;

            const char *staggerDelay = stag->time ? stag->time : "100ms";

            char staggerPrefix[256];
            snprintf(staggerPrefix, sizeof staggerPrefix, "%s-stagger-%u", prefix, _uniqueId++);

            // Generate stagger timing classes
            _out(f, 0, ".%s {", staggerPrefix);
            _out(f, 1, "animation-fill-mode: forwards;");
            _out(f, 0, "}");

            for (int i = 1; i <= 10; ++i) {
                _out(f, 0, ".%s:nth-child(%d) {", staggerPrefix, i);
                if (strstr(staggerDelay, "ms")) {
                    int delayMs = atoi(staggerDelay) * (i - 1);
                    _out(f, 1, "animation-delay: %dms;", delayMs);
                } else if (strstr(staggerDelay, "s")) {
                    float delayS = atof(staggerDelay) * (i - 1);
                    _out(f, 1, "animation-delay: %.2fs;", delayS);
                } else {
                    int delayMs = 100 * (i - 1);
                    _out(f, 1, "animation-delay: %dms;", delayMs);
                }
                _out(f, 0, "}");
            }

            // Process nested step items within the stagger
            if (stag->stepItemList) {
                for (size_t i = 0; i < stag->stepItemList->itemCount; ++i)
                    _genStepItem(f, staggerPrefix, stag->stepItemList->items[i]);
            }
            break;
        }

        default:
            logError(_logger, "Unknown StepItem type %d", step->type);
    }
}

static void _genTransitionCss(FILE *f, Trigger *t, Transition *tr)
{
    if (!tr || !tr->transitionRule || !tr->transitionBlock) return;

    int  bidir = 0;
    const char *from = NULL, *to = NULL;

    if (tr->transitionRule->ruleType == FROM_TO) {
        from  = tr->transitionRule->fromState;
        to    = tr->transitionRule->toState;
        bidir = (tr->transitionRule->direction == BIDIRECTIONAL);
    } else {
        if (tr->transitionRule->alias == ENTER)      { from = "void"; to = "*";   }
        else if (tr->transitionRule->alias == LEAVE) { from = "*";    to = "void"; }
        else {
            const char *aliasStr = _aliasToState(tr->transitionRule->alias);
            from = to = aliasStr;
        }
    }

    Style *origSt = NULL, *destSt = NULL;
    if (t->block && t->block->stateList) {
        for (size_t i = 0; i < t->block->stateList->stateCount; ++i) {
            State *s = t->block->stateList->states[i];
            if (from && strcmp(s->name, from) == 0) origSt = s->style;
            if (to   && strcmp(s->name, to)   == 0) destSt = s->style;
        }
    }

    for (int pass = 0; pass < (bidir ? 2 : 1); ++pass) {
        const char *o = (pass == 0 ? from : to);
        const char *d = (pass == 0 ? to   : from);

        char prefix[128];
        if (strcmp(o, "void") == 0 && strcmp(d, "*") == 0) {
            snprintf(prefix, sizeof prefix, "%s-enter", t->name);
        } else if (strcmp(o, "*") == 0 && strcmp(d, "void") == 0) {
            snprintf(prefix, sizeof prefix, "%s-leave", t->name);
        } else {
            snprintf(prefix, sizeof prefix, "%s-%s-to-%s", t->name, _sanitize(o), _sanitize(d));
        }

        StepItemList *sil = tr->transitionBlock->stepItemList;

        for (size_t i = 0; i < sil->itemCount; ++i) {
            StepItem *si = sil->items[i];
            if (!si) continue;
            _genStepItem(f, prefix, si);
        }
    }
}

static void _genTriggerCss(FILE *f, Trigger *t)
{
    if (!t || !t->block) return;

    logDebugging(_logger, "Trigger `%s`…", t->name);

    if (t->block->stateList) {
        for (size_t i = 0; i < t->block->stateList->stateCount; ++i)
            _genStateClassCss(f, t->name, t->block->stateList->states[i]);
    }

    if (t->block->transitionList) {
        for (size_t i = 0; i < t->block->transitionList->transitionCount; ++i)
            _genTransitionCss(f, t, t->block->transitionList->transitions[i]);
    }
}

static void _genProgramCss(FILE *f, Program *p)
{
    if (!p || !p->triggerList) return;

    for (size_t i = 0; i < p->triggerList->triggerCount; ++i)
        _genTriggerCss(f, p->triggerList->trigger[i]);
}