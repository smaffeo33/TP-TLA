#include "Animato.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

static Logger *_logger = NULL;

void initializeSemanticValidatorModule(void)  { _logger = createLogger("Semantics"); }
void shutdownSemanticValidatorModule(void)    { if (_logger) destroyLogger(_logger); }

static ComputationResult invalidComputation(void);
static boolean isDurationAndValid(const char *txt);
static boolean isEasing(const char *txt);

static ComputationResult validateTrigger(Trigger *t);
static boolean validateState(Trigger *parent, State *s, SymbolTable *symbols);
static boolean validateTrans(Trigger *parent, Transition *tr, SymbolTable *symbols);

#define MAX_DURATION_MS 60000
#define MAX_DURATION_S 60.0


ComputationResult validateProgram(Program *program, SymbolTable * globalSymbols)
{
    if (!program || !program->triggerList) {
        logError(_logger, "Program or trigger list is NULL.");
        return invalidComputation();
    }

    size_t errorCount = 0;

    for (size_t i = 0; i < program->triggerList->triggerCount; ++i) {
        Trigger *t = program->triggerList->trigger[i];

        if (!stAdd(globalSymbols, t->name, SYMBOL_TRIGGER)) {
            logError(_logger, "Duplicate trigger '%s'.", t->name);
            ++errorCount;
        }

        ComputationResult cr = validateTrigger(t);
        if (!cr.succeed) errorCount += cr.value;
    }

    return (ComputationResult){ .succeed = errorCount == 0, .value = (int) errorCount };
}

static ComputationResult validateTrigger(Trigger *t)
{
    if (!t) return invalidComputation();

    SymbolTable symbols;
    stInit(&symbols);
    size_t errorCount = 0;
    stAdd(&symbols, "*", SYMBOL_STATE); //predefino wildcard y void estados, asegurando que nadie defina uno nuevo.
    stAdd(&symbols, "void", SYMBOL_STATE);


    for (size_t i = 0; i < t->block->stateList->stateCount; ++i) {
        if (!validateState(t, t->block->stateList->states[i], &symbols))
            ++errorCount;
    }

    for (size_t i = 0; i < t->block->transitionList->transitionCount; ++i) {
        if (!validateTrans(t, t->block->transitionList->transitions[i], &symbols))
            ++errorCount;
    }

    stDestroy(&symbols);

    return (ComputationResult){ .succeed = errorCount == 0, .value = (int) errorCount };
}

static boolean validateState(Trigger *parent, State *s, SymbolTable *symbols)
{
    if (!s) {
        logError(_logger, "NULL state in trigger '%s'.", parent->name);
        return false;
    }

    if (!stAdd(symbols, s->name, SYMBOL_STATE)) {
        logError(_logger, "Duplicate state '%s' in trigger '%s'.", s->name, parent->name);
        return false;
    }

    return true;
}

static boolean validateTrans(Trigger *parent, Transition *tr, SymbolTable *symbols)
{
    if (!tr || !symbols) return false;

    const char *from = NULL;
    const char *to   = NULL;

    if (tr->transitionRule->ruleType == FROM_TO) {
        from = tr->transitionRule->fromState;
        to   = tr->transitionRule->toState;

        if (!stContains(symbols, from)) {
            logError(_logger, "State '%s' not declared (trigger %s).", from, parent->name);
            return false;
        }

        if (!stContains(symbols, to)) {
            logError(_logger, "State '%s' not declared (trigger %s).", to, parent->name);
            return false;
        }

        // Validar que no haya transición duplicada
        char transitionKey[256];
        snprintf(transitionKey, sizeof(transitionKey), "%s->%s", from, to);
        if (!stAdd(symbols, transitionKey, SYMBOL_TRANSITION)) {
            logError(_logger, "Duplicate transition '%s' in trigger %s.", transitionKey, parent->name);
            return false;
        }
    }

    StepItem *si = tr->transitionBlock->stepItemList->items[0];
    Animate *an = (Animate *) si->item;
    AnimateInfo *ai = an->animateInfo;

    if (!isDurationAndValid(ai->duration)) {
        logError(_logger, "Invalid duration '%s' in trigger %s.", ai->duration, parent->name);
        return false;
    }

    if (ai->easing && !isEasing(ai->easing)) {
        logError(_logger, "Unsupported easing '%s' in trigger %s.", ai->easing, parent->name);
        return false;
    }

    return true;
}

static boolean isDurationAndValid(const char *txt) //validar si es una duracion y si esta es valida
{
    if (!txt) return false;

    size_t len = strlen(txt);
    if (len < 3) return false;

    const char *unit = &txt[len - 2];
    char suffix[3];
    strncpy(suffix, unit, 2);
    suffix[2] = '\0';

    if (strcmp(suffix, "ms") != 0 && txt[len - 1] != 's')
        return false;

    char numberPart[32];
    size_t numberLen = len - ((strcmp(suffix, "ms") == 0) ? 2 : 1);
    if (numberLen >= sizeof(numberPart)) return false;

    strncpy(numberPart, txt, numberLen);
    numberPart[numberLen] = '\0';

    char *endptr;
    double value = strtod(numberPart, &endptr);
    if (*endptr != '\0' || value < 0.0)
        return false;

    // Valor máximo razonable 60s o 60000ms
double max = (strcmp(suffix, "ms") == 0) ? MAX_DURATION_MS : MAX_DURATION_S;
    return value <= max;
}


static boolean isEasing(const char *txt)
{
    static const char *ok[] = {"linear", "ease", "ease-in", "ease-out", "ease-in-out"};
    for (size_t i = 0; i < sizeof(ok) / sizeof(*ok); ++i)
        if (strcmp(txt, ok[i]) == 0) return true;

    return txt && strncmp(txt, "cubic-bezier(", 13) == 0;
}


static ComputationResult invalidComputation(void)
{
    return (ComputationResult){ .succeed = false, .value = 0 };
}
