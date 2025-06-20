#include "Generator.h"
#include "../../frontend/syntactic-analysis/AbstractSyntaxTree.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

static Logger *_logger = NULL;
const  char    _indentationCharacter = ' ';
const  unsigned _indentationSize     = 4;

static unsigned _uniqueId = 0;

void initializeGeneratorModule(void)  { _logger = createLogger("Generator"); }
void shutdownGeneratorModule(void)    { destroyLogger(_logger); }

static void   _printIndent(FILE *f, unsigned lvl);
static void   _out       (FILE *f, unsigned lvl, const char *fmt, ...);
static void   _genProgramCss (FILE *f, Program *p);
static void   _genTriggerCss (FILE *f, Trigger *t);
static void   _genStateClassCss(FILE *f, Trigger *t, State *s);
static void   _genStylePropsCss(FILE *f, unsigned lvl, PropertyList *plist);
static void   _genStepItem(FILE *f, const char *prefix, StepItem *step);
static const char *_aliasToState(AliasType a);

#define DEF_BUF 256

void generate(CompilerState * compilerState)
{
    if (!compilerState || !compilerState->abstractSyntaxtTree) {
        logError(_logger, "Compiler state or AST is NULL – nothing to generate.");
        return;
    }
    logDebugging(_logger, "Generating CSS for the program...");
    _genProgramCss(stdout, compilerState->abstractSyntaxtTree);
}

static void _printIndent(FILE *f, unsigned lvl)
{
    for (unsigned i = 0; i < lvl * _indentationSize; ++i)
        fputc(_indentationCharacter, f);
}

static void _out(FILE *f, unsigned lvl, const char *fmt, ...)
{
    _printIndent(f, lvl);
    va_list ap;  va_start(ap, fmt);
    vfprintf(f, fmt, ap);
    va_end(ap);
}

static void _genStylePropsCss(FILE *f, unsigned lvl, PropertyList *plist)
{
    if (!plist) return;
    for (size_t i = 0; i < plist->propertyCount; ++i) {
        Property *p = plist->properties[i];
        switch (p->type) {
            case STRING:
            case COLOR:
                _out(f, lvl, "%s: %s;\n", p->name, p->value);
                break;
            case UNIT:
                if (p->value)
                    _out(f, lvl, "%s: %s;\n", p->name, p->value);
                else
                    _out(f, lvl, "%s: %.2f;\n", p->name, p->floatValue);
                break;
            case UNITLESS:
                _out(f, lvl, "%s: %.3g;\n", p->name, p->floatValue);
                break;
        }
    }
}

static void _genStateClassCss(FILE *f, Trigger *t, State *s)
{
    _out(f, 0, ".%s-%s {\n", t->name, s->name);
    _genStylePropsCss(f, 1, s->style->properties);
    _out(f, 0, "}\n");
}

static const char *_aliasToState(AliasType a)
{
    switch (a) {
        case ENTER:     return "enter";
        case LEAVE:     return "leave";
        case INCREMENT: return "increment";
        case DECREMENT: return "decrement";
        default:        return "state";
    }
}

static void _genAnimate(FILE *f, const char *prefix, Animate *a, Style *fromStyle, Style *toStyle)
{
    if (!f || !a || !prefix) return;

    AnimateInfo *ai = a->animateInfo;
    const char *dur = ai && ai->duration ? ai->duration : "0ms";
    const char *eas = ai && ai->easing   ? ai->easing   : "ease";
    const char *del = ai && ai->delay    ? ai->delay    : "0ms";
    int hasDelay = ai && ai->delay && strcmp(ai->delay, "0ms") != 0 && strcmp(ai->delay, "0") != 0;

    char kfName[DEF_BUF];
    snprintf(kfName, sizeof kfName, "%s-kf-%u", prefix, _uniqueId++);

    if (a->type == ANIMATE_WITH_KEYFRAMES && a->keyframes && a->keyframes->keyframeStyleList) {
        _out(f, 0, "@keyframes %s {\n", kfName);
        KeyframeStyleList *list = a->keyframes->keyframeStyleList;
        for (size_t i = 0; i < list->keyframeCount; ++i) {
            KeyframeStyle *k = list->keyframeStyles[i];
            _out(f, 1, "%.0f%% {\n", k->offset * 100.0f);
            _genStylePropsCss(f, 2, k->properties);
            _out(f, 1, "}\n");
        }
        _out(f, 0, "}\n");

    } else {
        _out(f, 0, "@keyframes %s {\n", kfName);

        _out(f, 1, "from {\n");
        if (fromStyle && fromStyle->properties) {
            _genStylePropsCss(f, 2, fromStyle->properties);
        } else if (a->type == ANIMATE_WITH_STYLE && a->style && a->style->properties) {
            _genStylePropsCss(f, 2, a->style->properties);
        }
        _out(f, 1, "}\n");

        _out(f, 1, "to {\n");

        if (toStyle && toStyle->properties) {
            _genStylePropsCss(f, 2, toStyle->properties);
        } else if (a->type == ANIMATE_WITH_STYLE && a->style && a->style->properties) {
            _genStylePropsCss(f, 2, a->style->properties);
        } else if (fromStyle && fromStyle->properties) {
            _genStylePropsCss(f, 2, fromStyle->properties);
        }

        _out(f, 1, "}\n");
        _out(f, 0, "}\n");
    }

    _out(f, 0, ".%s {\n", prefix);
    if (hasDelay)
        _out(f, 1, "animation: %s %s %s %s forwards;\n", kfName, dur, eas, del);
    else
        _out(f, 1, "animation: %s %s %s forwards;\n", kfName, dur, eas);
    _out(f, 0, "}\n");
}

static void _genStepItem(FILE *f, const char *prefix, StepItem *stepItem)
{
    switch (stepItem->type) {
        case ANIMATE_ITEM:
            _genAnimate(f, prefix, (Animate *)stepItem->item, NULL, NULL);
            break;

        case STYLE_ITEM: {
            Style *s = (Style *)stepItem->item;
            _out(f, 0, ".%s-style-%u {\n", prefix, _uniqueId++);
            _genStylePropsCss(f, 1, s->properties);
            _out(f, 0, "}\n");
            break;
        }

        case GROUP_ITEM:
        case SEQUENCE_ITEM: {
            StepItemList *stepItemList = ((stepItem->type == GROUP_ITEM)
                                          ? ((Group *)stepItem->item)->stepItemList
                                          : ((Sequence *)stepItem->item)->stepItemList);
            for (size_t i = 0; i < stepItemList->itemCount; ++i)
                _genStepItem(f, prefix, stepItemList->items[i]);
            break;
        }

        case QUERY_ITEM: {
            Query *q = (Query *)stepItem->item;
            StepItemList *stepItemList = q->stepItemList;
            for (size_t i = 0; i < stepItemList->itemCount; ++i) {
                char nestedPrefix[DEF_BUF];
                snprintf(nestedPrefix, sizeof nestedPrefix, "%s-query-%s", prefix,
                         (q->selectorType == ALIAS_SELECTOR) ? _aliasToState(q->alias) : q->selector);
                _genStepItem(f, nestedPrefix, stepItemList->items[i]);
            }
            break;
        }

        case STAGGER_ITEM: {
            logWarning(_logger, "stagger() encountered – not yet supported in CSS generator.");
            break;
        }

        case ANIMATE_CHILD_ITEM:
            logWarning(_logger, "animateChild() encountered – not representable in static CSS.");
            break;

        default:
            logError(_logger, "Unknown StepItem type.");
    }
}
static void _genTransitionCss(FILE *f, Trigger *t, Transition *tr)
{
    if (!f || !t || !tr || !tr->transitionRule || !tr->transitionBlock) return;

    const char *from, *to;
    int bidir = 0;

    if (tr->transitionRule->ruleType == FROM_TO) {
        from = tr->transitionRule->fromState;
        to   = tr->transitionRule->toState;
        bidir = (tr->transitionRule->direction == BIDIRECTIONAL);
    } else {
        from = to = _aliasToState(tr->transitionRule->alias);
    }

    for (int pass = 0; pass < (bidir ? 2 : 1); ++pass) {
        const char *orig = (pass == 0) ? from : to;
        const char *dest = (pass == 0) ? to   : from;

        // Buscar los estilos definidos para los estados
        Style *origStyle = NULL;
        Style *destStyle = NULL;
        if (t->block && t->block->stateList && t->block->stateList->states) {
            for (size_t i = 0; i < t->block->stateList->stateCount; ++i) {
                State *s = t->block->stateList->states[i];
                if (s && s->name) {
                    if (strcmp(s->name, orig) == 0) origStyle = s->style;
                    if (strcmp(s->name, dest) == 0) destStyle = s->style;
                }
            }
        }

        char prefix[DEF_BUF];
        snprintf(prefix, sizeof prefix, "%s-%s-to-%s", t->name, orig, dest);

        StepItemList *sil = tr->transitionBlock->stepItemList;
        if (!sil || !sil->items) continue;

        Style *lastStyleSeen = NULL;

        for (size_t i = 0; i < sil->itemCount; ++i) {
            StepItem *stepItem = sil->items[i];
            if (!stepItem) continue;

            switch (stepItem->type) {
                case STYLE_ITEM:
                    lastStyleSeen = (Style *)stepItem->item;
                    break;

                case ANIMATE_ITEM: {
                    Animate *a = (Animate *)stepItem->item;
                    if (!a) continue;

                    Style *effectiveFrom = lastStyleSeen ? lastStyleSeen : origStyle;

                    Style *effectiveTo = destStyle;
                    if (!destStyle && lastStyleSeen == NULL && a->type != ANIMATE_WITH_STYLE && a->type != ANIMATE_WITH_KEYFRAMES) {
                        static Style emptyStyle = { .properties = NULL };
                        effectiveTo = &emptyStyle;
                    }

                    _genAnimate(f, prefix, a, effectiveFrom, effectiveTo);

                    lastStyleSeen = NULL;
                    break;
                }


                default:
                    _genStepItem(f, prefix, stepItem);
                    break;
            }
        }
    }
}



static void _genTriggerCss(FILE *f, Trigger *t)
{
    if (!t || !t->block) return;

    logDebugging(_logger, "Generating CSS for trigger: %s", t->name);

    StateList *sl = t->block->stateList;
    if (sl && sl->states) {
        for (size_t i = 0; i < sl->stateCount; ++i) {
            if (sl->states[i]) _genStateClassCss(f, t, sl->states[i]);
        }
    }

    _out(f, 0, "\n");

    TransitionList *tl = t->block->transitionList;
    if (tl && tl->transitions) {
        for (size_t i = 0; i < tl->transitionCount; ++i) {
            if (tl->transitions[i]) {
                _genTransitionCss(f, t, tl->transitions[i]);
                _out(f, 0, "\n");
            }
        }
    }
}


static void _genProgramCss(FILE *f, Program *p)
{
    if (!p || !p->triggerList || !p->triggerList->trigger)
        return;

    for (size_t i = 0; i < p->triggerList->triggerCount; ++i) {
        Trigger *t = p->triggerList->trigger[i];
        if (t) _genTriggerCss(f, t);
    }
}

