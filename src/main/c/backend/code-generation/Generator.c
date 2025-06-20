#include "Generator.h"
#include "../../frontend/syntactic-analysis/AbstractSyntaxTree.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

static Logger *_logger = NULL;
static const char _indentChar = ' ';
static const unsigned _indentSize = 4;
static unsigned _uniqueId = 0;

/* --------------------------------------------------------------------------
 * Forward declarations
 * -------------------------------------------------------------------------- */
static void   _printIndent(FILE *f, unsigned lvl);
static void   _out       (FILE *f, unsigned lvl, const char *fmt, ...);
static const char *_sanitize(const char *name);
static const char *_aliasToState(AliasType a);
static void   _genProgramCss   (FILE *f, Program *p);
static void   _genTriggerCss   (FILE *f, Trigger *t);
static void   _genStateClassCss(FILE *f, const char *triggerName, State *s);
static void   _genStylePropsCss(FILE *f, unsigned lvl, PropertyList *plist);
static void   _genStepItem     (FILE *f, const char *prefix, StepItem *step);
static void   _genTransitionCss(FILE *f, Trigger *t, Transition *tr);
static void   _genAnimateCss   (FILE *f, const char *prefix, Animate *a,
                                Style *fromStyle, Style *toStyle);

/* --------------------------------------------------------------------------
 * Public API
 * -------------------------------------------------------------------------- */
void initializeGeneratorModule(void)  { _logger = createLogger("Generator"); }
void shutdownGeneratorModule(void)    { destroyLogger(_logger);            }

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
}

static const char *_sanitize(const char *name)
{
    if (!name) return "";
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
        Property *p = plist->properties ? plist->properties[i] : NULL;
        if (!p || !p->name) continue;               /* Robust‑guard against NULL */

        const char *name = p->name;

        /* Map Angular 'scale' to CSS transform */
        if (strcmp(name, "scale") == 0) {
            char buf[64] = "1";                   /* default scale(1) */
            switch (p->type) {
                case UNITLESS:
                    snprintf(buf, sizeof buf, "%.3g", p->floatValue);
                    break;
                case UNIT:
                case STRING:
                    if (p->value && *p->value)
                        snprintf(buf, sizeof buf, "%s", p->value);
                    break;
                default:
                    break;
            }
            _out(f, lvl, "transform: scale(%s);", buf);
            continue;
        }

        /* Normal property emission */
        switch (p->type) {
            case STRING:
            case COLOR:
                if (p->value)
                    _out(f, lvl, "%s: %s;", name, p->value);
                break;
            case UNIT:
                if (p->value && *p->value)
                    _out(f, lvl, "%s: %s;", name, p->value);
                else
                    _out(f, lvl, "%s: %.2f;", name, p->floatValue);
                break;
            case UNITLESS:
                _out(f, lvl, "%s: %.3g;", name, p->floatValue);
                break;
            default:
                break;
        }
    }
}

static void _genStateClassCss(FILE *f, const char *triggerName, State *s)
{
    const char *san = _sanitize(s->name);

    if (strcmp(san, "void") == 0) return;

    _out(f, 0, ".%s-%s {\n", triggerName, san);
    _genStylePropsCss(f, 1, s->style->properties);
    _out(f, 0, "}\n\n");
}


static void _genAnimateCss(FILE *f,
                           const char *prefix,
                           Animate    *a,
                           Style      *fromStyle,
                           Style      *toStyle)
{
    if (!a) return;

    /* === 1. Timing ==================================================== */
    AnimateInfo *ai = a->animateInfo;
    const char *dur = ai && ai->duration ? ai->duration : "0ms";
    const char *eas = ai && ai->easing   ? ai->easing   : "ease";
    const char *del = ai && ai->delay    ? ai->delay    : "0ms";
    int   hasDelay = ai && ai->delay && strcmp(del, "0ms") != 0 && strcmp(del,"0")!=0;

    /* === 2. Unique names ============================================== */
    unsigned id = _uniqueId++;
    char kfName[128];  snprintf(kfName,  sizeof kfName,  "%s-kf-%u",   prefix, id);
    char clsName[256]; snprintf(clsName, sizeof clsName, "%s-anim-%u", prefix, id);

    /* === 3. Prep destination‑property list ============================ */
    PropertyList *destProps = NULL;
    if (toStyle)                   destProps = toStyle->properties;
    else if (a->type == ANIMATE_WITH_STYLE && a->style)
        destProps = a->style->properties;

    /* === 4. @keyframes ================================================ */
    _out(f, 0, "@keyframes %s {", kfName);

    if (a->type == ANIMATE_WITH_KEYFRAMES && a->keyframes) {
        /* Copy user‑defined keyframes verbatim */
        KeyframeStyleList *kl = a->keyframes->keyframeStyleList;
        for (size_t i = 0; i < kl->keyframeCount; ++i) {
            KeyframeStyle *ks = kl->keyframeStyles[i];
            float pct = ks->offset <= 1.f ? ks->offset * 100.f : ks->offset;
            _out(f, 1, "%.0f%% {", pct);
            _genStylePropsCss(f, 2, ks->properties);
            _out(f, 1, "}");
        }
    } else {
        /* ------- FROM block ----------------------------------------- */
        _out(f, 1, "from {");
        if (fromStyle) {
            _genStylePropsCss(f, 2, fromStyle->properties);
        } else if (destProps) {
            /* Provide sensible defaults per property animated */
            for (size_t i = 0; i < destProps->propertyCount; ++i) {
                Property *p = destProps->properties[i];
                if (!p || !p->name) continue;
                const char *name = p->name;

                if (strcmp(name, "opacity") == 0) {
                    _out(f, 2, "opacity: %s;",
                    strstr(prefix,"enter") ? "0" : "1");
                } else if (strcmp(name, "height") == 0) {
                    _out(f, 2, "height: %s;",
                    strstr(prefix,"enter") ? "0" : "auto");
                } else if (strcmp(name, "scale") == 0) {
                    _out(f, 2, "transform: scale(1);");
                }
            }
        }
        _out(f, 1, "}");

        /* ------- TO block ------------------------------------------- */
        _out(f, 1, "to {");
        if (toStyle) {
            _genStylePropsCss(f, 2, toStyle->properties);
        } else if (a->type == ANIMATE_WITH_STYLE && a->style) {
            _genStylePropsCss(f, 2, a->style->properties);
        } else {
            /* Fallback generic defaults */
            if (strstr(prefix, "enter")) {
                _out(f, 2, "opacity: 1;");
                _out(f, 2, "height: auto;");
            } else if (strstr(prefix, "leave")) {
                _out(f, 2, "opacity: 0;");
                _out(f, 2, "height: 0;");
            }
        }
        _out(f, 1, "}");
    }
    _out(f, 0, "}");

    /* === 5. Utility class ============================================ */
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
            _out(f, 0, ".%s {\n", tmp);
            _genStylePropsCss(f, 1, st->properties);
            _out(f, 0, "}\n\n");
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
                              : q->selector;
            char nprefix[256];
            snprintf(nprefix, sizeof nprefix, "%s-query-%s", prefix, sel);
            for (size_t i = 0; i < q->stepItemList->itemCount; ++i)
                _genStepItem(f, nprefix, q->stepItemList->items[i]);
            break;
        }

        case ANIMATE_CHILD_ITEM: {
            static char lastChildPrefix[256] = {0};
            if (strcmp(lastChildPrefix, prefix) != 0) {
                strcpy(lastChildPrefix, prefix);

                char childPrefix[256];
                snprintf(childPrefix, sizeof childPrefix, "%s-child-anim", prefix);

                _out(f, 0, "@keyframes %s-kf {\n", childPrefix);
                _out(f, 1, "from {\n");
                _out(f, 2, "opacity: 0;\n");
                _out(f, 2, "transform: translateY(10px);\n");
                _out(f, 1, "}\n");
                _out(f, 1, "to {\n");
                _out(f, 2, "opacity: 1;\n");
                _out(f, 2, "transform: translateY(0);\n");
                _out(f, 1, "}\n");
                _out(f, 0, "}\n");

                _out(f, 0, ".%s > * {\n", prefix);
                _out(f, 1, "animation: %s-kf 0.3s ease-out forwards;\n", childPrefix);
                _out(f, 0, "}\n\n");
            }
            break;
        }

        case STAGGER_ITEM: {
            Stagger *stag = (Stagger *)step->item;
            if (!stag) break;

            const char *staggerDelay = stag->time ? stag->time : "100ms";

            char staggerPrefix[256];
            snprintf(staggerPrefix, sizeof staggerPrefix, "%s-stagger-%u", prefix, _uniqueId++);

            _out(f, 0, "/* Stagger animation with %s delay */\n", staggerDelay);
            _out(f, 0, ".%s {\n", staggerPrefix);
            _out(f, 1, "animation-fill-mode: forwards;\n");
            _out(f, 0, "}\n");

            for (int i = 1; i <= 10; ++i) {
                _out(f, 0, ".%s:nth-child(%d) {\n", staggerPrefix, i);
                if (strstr(staggerDelay, "ms")) {
                    int delayMs = atoi(staggerDelay) * (i - 1);
                    _out(f, 1, "animation-delay: %dms;\n", delayMs);
                } else if (strstr(staggerDelay, "s")) {
                    float delayS = atof(staggerDelay) * (i - 1);
                    _out(f, 1, "animation-delay: %.2fs;\n", delayS);
                } else {
                    int delayMs = 100 * (i - 1);
                    _out(f, 1, "animation-delay: %dms;\n", delayMs);
                }
                _out(f, 0, "}\n");
            }
            _out(f, 0, "\n");

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
        Style *lastSt = NULL;

        for (size_t i = 0; i < sil->itemCount; ++i) {
            StepItem *si = sil->items[i];
            if (!si) continue;

            switch (si->type) {
                case STYLE_ITEM:
                    lastSt = (Style *)si->item;
                    break;

                case ANIMATE_ITEM: {
                    Animate *a      = (Animate *)si->item;
                    Style  *fromSty = lastSt ? lastSt : origSt;
                    Style  *toSty   = destSt ? destSt
                                             : (a->type == ANIMATE_WITH_STYLE ? a->style : NULL);
                    _genAnimateCss(f, prefix, a, fromSty, toSty);
                    lastSt = NULL;
                    break;
                }

                default:
                    _genStepItem(f, prefix, si);
            }
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

    /* Emit transitions */
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