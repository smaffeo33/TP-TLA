#include "Generator.h"
#include "../../frontend/syntactic-analysis/AbstractSyntaxTree.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>


static Logger *_logger = NULL;
const  char    _indentationCharacter = ' ';
const  unsigned _indentationSize     = 4;


void initializeGeneratorModule(void)  { _logger = createLogger("Generator"); }
void shutdownGeneratorModule(void)    { destroyLogger(_logger);  }


static void   _printIndent(FILE *f, unsigned lvl);
static void   _out       (FILE *f, unsigned lvl, const char *fmt, ...);
static void   _genProgramCss (FILE *f, Program *p);
static void   _genTriggerCss (FILE *f, Trigger *t);
static void   _genStateClassCss(FILE *f, Trigger *t, State *s);
static void   _genStylePropsCss(FILE *f, unsigned lvl, PropertyList *plist);
static void   _genKeyframesCss (FILE *f, const char *kfName, Keyframes *kfs);
static void   _genTransitionCss(FILE *f, Trigger *t, Transition *tr);

static const char *_aliasToState(AliasType a);
#define DEF_BUF 256


void generate(CompilerState * compilerState)
{
    if (!compilerState || !compilerState->abstractSyntaxtTree) {
        logError(_logger, "Compiler state or AST is NULL – nothing to generate.");
        return;
    }
    logDebugging(_logger, "Generating CSS for the program...");
    //Si queremos sacarlo a un archivo habria que cambiar el FILE *stdout por un FILE *f
    _genProgramCss(stdout,
                   compilerState->abstractSyntaxtTree);
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

static void _genKeyframesCss(FILE *f, const char *kfName, Keyframes *kfs)
{
    _out(f, 0, "@keyframes %s {\n", kfName);
    KeyframeStyleList *list = kfs->keyframeStyleList;
    for (size_t i = 0; i < list->keyframeCount; ++i) {
        KeyframeStyle *k = list->keyframeStyles[i];
        _out(f, 1, "%.0f%% {\n", k->offset * 100.0f);
        _genStylePropsCss(f, 2, k->properties);
        _out(f, 1, "}\n");
    }
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


static void _genTransitionCss(FILE *f, Trigger *t, Transition *tr)
{
    const char *from, *to;
    int bidir = false;                         /* default → sentido único */

    if (tr->transitionRule->ruleType == FROM_TO) {
        from   = tr->transitionRule->fromState;
        to     = tr->transitionRule->toState;
        bidir  = tr->transitionRule->direction;
    } else {
        const char *alias = _aliasToState(tr->transitionRule->alias);
        from = to = alias;
    }

    StepItem    *si  = tr->transitionBlock->stepItemList->items[0];
    Animate     *anim= (Animate *)si->item;
    AnimateInfo *ai  = anim->animateInfo;

    const char *dur = ai->duration ? ai->duration : "0ms";
    const char *eas = ai->easing   ? ai->easing   : "ease";
    const char *del = ai->delay    ? ai->delay    : "0ms";

    int withDelay = ai->delay && strcmp(ai->delay,"0ms")!=0 && strcmp(ai->delay,"0")!=0;

    for (int pass = 0; pass < (bidir ? 2 : 1); ++pass) {

        const char *orig = (pass == 0) ? from : to;
        const char *dest = (pass == 0) ? to   : from;

        Style *origStyle = NULL, *destStyle = NULL;
        if (!anim->style && !(anim->type == ANIMATE_WITH_KEYFRAMES && anim->keyframes)) {
            StateList *sl = t->block->stateList;
            for (size_t i = 0; i < sl->stateCount; ++i) {
                State *s = sl->states[i];
                if (strcmp(s->name, orig) == 0) origStyle = s->style;
                if (strcmp(s->name, dest) == 0) destStyle = s->style;
            }
        }

        /* 3b. Nombre de keyframes ------------------------------------- */
        char kfName[256];
        snprintf(kfName, sizeof kfName,"%s-%s-to-%s", t->name, orig, dest);

        if (anim->type == ANIMATE_WITH_KEYFRAMES && anim->keyframes) {
            _genKeyframesCss(f, kfName, anim->keyframes);

        } else if (anim->style) {
            _out(f,0,"@keyframes %s {\n", kfName);
            _out(f,1,"from {\n"); _genStylePropsCss(f,2,anim->style->properties); _out(f,1,"}\n");
            _out(f,1,"to   {\n"); _genStylePropsCss(f,2,anim->style->properties); _out(f,1,"}\n");
            _out(f,0,"}\n");

        } else {
            _out(f,0,"@keyframes %s {\n", kfName);
            _out(f,1,"from {\n"); _genStylePropsCss(f,2, origStyle ? origStyle->properties : NULL); _out(f,1,"}\n");
            _out(f,1,"to   {\n"); _genStylePropsCss(f,2, destStyle ? destStyle->properties : NULL); _out(f,1,"}\n");
            _out(f,0,"}\n");
        }

        _out(f,0,".%s-%s.%s-%s {\n", t->name, orig, t->name, dest);
        if (withDelay)
            _out(f,1,"animation: %s %s %s %s forwards;\n", kfName, dur, eas, del);
        else
            _out(f,1,"animation: %s %s %s forwards;\n",    kfName, dur, eas);
        _out(f,0,"}\n\n");
    }
}



static void _genTriggerCss(FILE *f, Trigger *t)
{
    logDebugging(_logger, "Generating CSS for trigger: %s", t->name);

    StateList *sl = t->block->stateList;
    for (size_t i = 0; i < sl->stateCount; ++i)
        _genStateClassCss(f, t, sl->states[i]);

    _out(f, 0, "\n");

    TransitionList *tl = t->block->transitionList;
    for (size_t i = 0; i < tl->transitionCount; ++i) {
        _genTransitionCss(f, t, tl->transitions[i]);
        _out(f, 0, "\n");
    }
}

static void _genProgramCss(FILE *f, Program *p)
{
    TriggerList *tl = p->triggerList;
    for (size_t i = 0; i < tl->triggerCount; ++i)
        _genTriggerCss(f, tl->trigger[i]);
}



