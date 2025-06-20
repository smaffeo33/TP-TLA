#include "Animato.h"
#include <string.h>
#include <stdlib.h>

static Logger *_logger = NULL;

void initializeSemanticValidatorModule(void)  { _logger = createLogger("Semantics"); }
void shutdownSemanticValidatorModule(void)    { if (_logger) destroyLogger(_logger); }

static ComputationResult _invalidComputation(void);
static boolean _isDuration (const char *txt);
static boolean _isEasing   (const char *txt);
static boolean _insertOnce (char ***array, size_t *count, const char *txt);

static ComputationResult _validateTrigger (Trigger *t);
static boolean           _validateState   (Trigger *parent, State *s,
                                           char ***names, size_t *count);
static boolean           _validateTrans   (Trigger *parent, Transition *tr,
                                           char **names,   size_t  count);

ComputationResult validateProgram(Program *program)
{
    if (!program || !program->triggerList) {
        logError(_logger, "Program or trigger list is NULL.");
        return _invalidComputation();
    }

    size_t errorCount = 0;

    for (size_t i = 0; i < program->triggerList->triggerCount; ++i) {
        ComputationResult cr = _validateTrigger(program->triggerList->trigger[i]);
        if (!cr.succeed) errorCount += cr.value;
    }

    ComputationResult result = {
            .succeed = errorCount == 0 ? true : false,
            .value   = (int) errorCount
    };
    return result;
}

static ComputationResult _validateTrigger(Trigger *t)
{
    if (!t) return _invalidComputation();

    size_t errorCount = 0;
    char **stateNames = NULL; size_t stateCount = 0;

//    for (size_t i = 0; i < t->block->stateList->stateCount; ++i)
//        if (!_validateState(t, t->block->stateList->states[i],
//                            &stateNames, &stateCount))
//            ++errorCount;
//
//    for (size_t i = 0; i < t->block->transitionList->transitionCount; ++i)
//        if (!_validateTrans(t, t->block->transitionList->transitions[i],
//                            stateNames, stateCount))
//            ++errorCount;

    free(stateNames);

    ComputationResult cr = {
            .succeed = errorCount == 0,
            .value   = (int) errorCount
    };
    return cr;
}

static boolean _validateState(Trigger *parent, State *s,
                              char ***names, size_t *count)
{
    if (!s) { logError(_logger, "NULL state in trigger %s.", parent->name); return false; }

    if (!_insertOnce(names, count, s->name)) {
        logError(_logger, "Duplicate state '%s' in trigger %s.", s->name, parent->name);
        return false;
    }
    return true;
}

static boolean _validateTrans(Trigger *parent, Transition *tr,
                              char **stateNames, size_t stateCount)
{
    const char *from, *to;

    if (tr->transitionRule->ruleType == FROM_TO) {
        from = tr->transitionRule->fromState;
        to   = tr->transitionRule->toState;
        boolean ok = false;
        for (size_t i=0;i<stateCount;++i)
            if (strcmp(stateNames[i], from) == 0) { ok = true; break; }
        if (!ok) {
            logError(_logger,"State '%s' not declared (trigger %s).",from,parent->name);
            return false;
        }
        ok = false;
        for (size_t i=0;i<stateCount;++i)
            if (strcmp(stateNames[i], to) == 0) { ok = true; break; }
        if (!ok) {
            logError(_logger,"State '%s' not declared (trigger %s).",to,parent->name);
            return false;
        }
    }

    StepItem *si = tr->transitionBlock->stepItemList->items[0];
    Animate  *an = (Animate *) si->item;
    AnimateInfo *ai = an->animateInfo;

    if (!_isDuration(ai->duration)) {
        logError(_logger,"Invalid duration '%s' in trigger %s.",ai->duration,parent->name);
        return false;
    }
    if (ai->easing && !_isEasing(ai->easing)) {
        logError(_logger,"Unsupported easing '%s' in trigger %s.",ai->easing,parent->name);
        return false;
    }
    return true;
}

static boolean _isDuration(const char *txt)
{
    if (!txt) return false;
    size_t n = strlen(txt);
    return (n > 2 && (txt[n-1]=='s') &&
            (txt[n-2]=='m' || txt[n-2] == '0' || txt[n-2] == '5'));
}

static boolean _isEasing(const char *txt)
{
    static const char *ok[] = {"linear","ease","ease-in","ease-out","ease-in-out"};
    for (size_t i=0;i<sizeof(ok)/sizeof(*ok);++i)
        if (strcmp(txt, ok[i]) == 0) return true;
    return strncmp(txt,"cubic-bezier(",13)==0;
}

static boolean _insertOnce(char ***array, size_t *count, const char *txt)
{
    for (size_t i=0;i<*count;++i)
        if (strcmp((*array)[i], txt)==0) return false;

    *array = realloc(*array, (*count + 1) * sizeof(char*));
    (*array)[*count] = strdup(txt);
    ++(*count);
    return true;
}

static ComputationResult _invalidComputation(void)
{
    ComputationResult cr = { .succeed = false, .value = 0 };
    return cr;
}
