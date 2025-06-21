#include "Animato.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "../../shared/Logger.h"
#include "../../shared/SymbolTable.h"

static Logger *_logger = NULL;

void initializeSemanticValidatorModule(void)  { _logger = createLogger("Semantics"); }
void shutdownSemanticValidatorModule(void)    { if (_logger) destroyLogger(_logger); }

static ComputationResult _invalidComputation(void);
static boolean _isDurationAndValid(const char *txt);
static boolean _isEasing(const char *txt);

static ComputationResult _validateTrigger(Trigger *t, SymbolTable *subTable,
                                          SymbolTable *globalSymbols);
static boolean _validateState(Trigger *parent, State *s, SymbolTable *symbols);
static boolean _validateAnimate(Animate *a, Trigger *parent);
static boolean _validateTrans(Trigger *parent, Transition *tr, SymbolTable *symbols,
                              SymbolTable *globalSymbols);
static boolean _validateQuery(Query *q, Trigger *parent, SymbolTable *globalSymbols);
static boolean _validateStagger(Stagger *s, Trigger *parent, SymbolTable *globalSymbols);
static boolean _validateStepItem(StepItem *si, Trigger *parent, SymbolTable *globalSymbols);
static boolean _validateGroup(Group *g, Trigger *parent, SymbolTable *globalSymbols);
static boolean _validateSequence(Sequence *seq, Trigger *parent, SymbolTable *globalSymbols);
static boolean _validateAnimateChild(StepItem *si, Trigger *parent, SymbolTable *globalSymbols);


#define MAX_DURATION_MS 60000
#define MAX_DURATION_S  60.0

ComputationResult validateProgram(Program *program, SymbolTable *globalSymbols)
{
    if (!program || !program->triggerList) {
        logError(_logger, "Program or trigger list is NULL.");
        return _invalidComputation();
    }

    size_t errorCount = 0;

    for (size_t i = 0; i < program->triggerList->triggerCount; ++i) {
        Trigger *t = program->triggerList->trigger[i];

        if (!stAdd(globalSymbols, t->name, SYMBOL_TRIGGER)) {
            logError(_logger, "Duplicate trigger '%s'.", t->name);
            ++errorCount;
            continue;
        }

        // Obtener el símbolo recién agregado
        Symbol *triggerSym = stGet(globalSymbols, t->name);
        if (!triggerSym) {
            logError(_logger, "Internal error: symbol for trigger '%s' not found.", t->name);
            ++errorCount;
            continue;
        }

        // Crear y asociar la subtabla
        triggerSym->subTable = malloc(sizeof(SymbolTable));
        stInit(triggerSym->subTable);

        ComputationResult cr = _validateTrigger(t, triggerSym->subTable, globalSymbols);
        if (!cr.succeed) errorCount += cr.value;
    }

    return (ComputationResult){ .succeed = errorCount == 0, .value = (int) errorCount };
}

static ComputationResult _validateTrigger(Trigger *t, SymbolTable *symbols, SymbolTable *globalSymbols)
{
    if (!t || !symbols) return _invalidComputation();

    stAdd(symbols, "*", SYMBOL_STATE);   // wildcard
    stAdd(symbols, "void", SYMBOL_STATE);

    size_t errorCount = 0;

    for (size_t i = 0; i < t->block->stateList->stateCount; ++i) {
        if (!_validateState(t, t->block->stateList->states[i], symbols))
            ++errorCount;
    }

    for (size_t i = 0; i < t->block->transitionList->transitionCount; ++i) {
        if (!_validateTrans(t, t->block->transitionList->transitions[i], symbols, globalSymbols))
            ++errorCount;
    }

    return (ComputationResult){ .succeed = errorCount == 0, .value = (int) errorCount };
}

static boolean _validateState(Trigger *parent, State *s, SymbolTable *symbols)
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

static boolean _validateTrans(Trigger *parent, Transition *tr, SymbolTable *symbols, SymbolTable *globalSymbols)
{
    if (!tr || !symbols) return false;

    const char *from = NULL;
    const char *to = NULL;

    if (tr->transitionRule->ruleType == FROM_TO) {
        from = tr->transitionRule->fromState;
        to = tr->transitionRule->toState;

        if (!stContains(symbols, from)) {
            logError(_logger, "State '%s' not declared (trigger '%s').", from, parent->name);
            return false;
        }

        if (!stContains(symbols, to)) {
            logError(_logger, "State '%s' not declared (trigger '%s').", to, parent->name);
            return false;
        }

        char transitionKey[256];
        snprintf(transitionKey, sizeof(transitionKey), "%s->%s", from, to);
        if (!stAdd(symbols, transitionKey, SYMBOL_TRANSITION)) {
            logError(_logger, "Duplicate transition '%s' in trigger '%s'.", transitionKey, parent->name);
            return false;
        }
    }

    StepItem *si = tr->transitionBlock->stepItemList->items[0];
    Animate *an = (Animate *) si->item;
    AnimateInfo *ai = an->animateInfo;

    if (!_isDurationAndValid(ai->duration)) {
        logError(_logger, "Invalid duration '%s' in trigger '%s'.", ai->duration, parent->name);
        return false;
    }

    if (ai->easing && !_isEasing(ai->easing)) {
        logError(_logger, "Unsupported easing '%s' in trigger '%s'.", ai->easing, parent->name);
        return false;
    }

    StepItemList *list = tr->transitionBlock->stepItemList;
    for (size_t i = 0; i < list->itemCount; ++i) {
        if (!_validateStepItem(list->items[i], parent, globalSymbols)) {
            return false;
        }
    }

    return true;
}

static boolean _validateStepItem(StepItem *si, Trigger *parent, SymbolTable *globalSymbols){
    if (!si) return false;

    switch (si->type) {
        case ANIMATE_ITEM:        return _validateAnimate(si->item, parent);
        case QUERY_ITEM:          return _validateQuery(si->item, parent, globalSymbols);
        case GROUP_ITEM:          return _validateGroup(si->item, parent, globalSymbols);
        case SEQUENCE_ITEM:       return _validateSequence(si->item, parent, globalSymbols);
        case STAGGER_ITEM:        return _validateStagger(si->item, parent, globalSymbols);
        case ANIMATE_CHILD_ITEM:  return _validateAnimateChild(si->item, parent, globalSymbols); // si tenés uno
        default:
            logError(_logger, "Unknown step type in trigger '%s'.", parent->name);
            return false;
    }
}

static boolean _validateAnimate(Animate *a, Trigger *parent) {
    if (!a || !a->animateInfo) return false;
    AnimateInfo *ai = a->animateInfo;

    if (!_isDurationAndValid(ai->duration)) {
        logError(_logger, "Invalid duration '%s' in trigger %s.", ai->duration, parent->name);
        return false;
    }

    if (ai->easing && !_isEasing(ai->easing)) {
        logError(_logger, "Unsupported easing '%s' in trigger %s.", ai->easing, parent->name);
        return false;
    }

    return true;
}

static boolean _validateQuery(Query *q, Trigger *parent, SymbolTable *globalSymbols) {
    if (!q || !q->stepItemList || q->stepItemList->itemCount == 0) {
        logError(_logger, "Empty or invalid query block in trigger '%s'.", parent->name);
        return false;
    }

    for (size_t i = 0; i < q->stepItemList->itemCount; ++i) {
        if (!_validateStepItem(q->stepItemList->items[i], parent,globalSymbols )) return false;
    }
    return true;
}

static boolean _validateStagger(Stagger *s, Trigger *parent, SymbolTable *globalSymbols) {
    if (!s || !s->time || !s->stepItemList) {
        logError(_logger, "Invalid stagger block in trigger '%s'.", parent->name);
        return false;
    }

    if (!_isDurationAndValid(s->time)) {
        logError(_logger, "Invalid stagger timing '%s' in trigger '%s'.", s->time, parent->name);
        return false;
    }

    for (size_t i = 0; i < s->stepItemList->itemCount; ++i) {
        if (!_validateStepItem(s->stepItemList->items[i], parent, globalSymbols)) return false;
    }
    return true;
}

static boolean _validateGroup(Group *g, Trigger *parent, SymbolTable *globalSymbols) {
    if (!g || !g->stepItemList || g->stepItemList->itemCount == 0) {
        logError(_logger, "Empty group block in trigger '%s'.", parent->name);
        return false;
    }

    for (size_t i = 0; i < g->stepItemList->itemCount; ++i) {
        if (!_validateStepItem(g->stepItemList->items[i], parent, globalSymbols)) return false;
    }
    return true;
}

static boolean _validateSequence(Sequence *seq, Trigger *parent, SymbolTable *globalSymbols) {
    if (!seq || !seq->stepItemList || seq->stepItemList->itemCount == 0) {
        logError(_logger, "Empty sequence block in trigger '%s'.", parent->name);
        return false;
    }

    for (size_t i = 0; i < seq->stepItemList->itemCount; ++i) {
        if (!_validateStepItem(seq->stepItemList->items[i], parent, globalSymbols)) return false;
    }
    return true;
}

static boolean _validateAnimateChild(StepItem *si, Trigger *parent, SymbolTable * globalSymbols) {
    if (!si || !parent) {
        logError(_logger, "animateChild() called in invalid context.");
        return false;
    }

    // Aquí podrías verificar que la estructura es correcta,
    // por ejemplo que no esté vacío o que el trigger tenga subTable.

    Symbol *triggerSym = stGet(globalSymbols, parent->name);
    if (!triggerSym || !triggerSym->subTable) {
        logError(_logger, "Trigger '%s' has no subTable; animateChild() might not be valid.", parent->name);
        // Esto es opcional, según el nivel de validación que quieras.
    }

    // Como no se puede verificar hijos explícitamente, acepta siempre.
    return true;
}




static boolean _isDurationAndValid(const char *txt)
{
    if (!txt) return false;

    size_t len = strlen(txt);
    if (len < 2) return false;

    const char *unit = &txt[len - 2];
    char suffix[3] = {0};
    strncpy(suffix, unit, 2);

    if (strcmp(suffix, "ms") != 0 && txt[len - 1] != 's')
        return false;

    size_t numberLen = len - ((strcmp(suffix, "ms") == 0) ? 2 : 1);
    if (numberLen >= 32) return false;

    char numberPart[32] = {0};
    strncpy(numberPart, txt, numberLen);

    char *endptr;
    double value = strtod(numberPart, &endptr);
    if (*endptr != '\0' || value < 0.0)
        return false;

    double max = (strcmp(suffix, "ms") == 0) ? MAX_DURATION_MS : MAX_DURATION_S;
    return value <= max;
}

static boolean _isEasing(const char *txt)
{
    static const char *ok[] = {"linear", "ease", "ease-in", "ease-out", "ease-in-out"};
    for (size_t i = 0; i < sizeof(ok) / sizeof(*ok); ++i)
        if (strcmp(txt, ok[i]) == 0) return true;

    return txt && strncmp(txt, "cubic-bezier(", 13) == 0;
}

static ComputationResult _invalidComputation(void)
{
    return (ComputationResult){ .succeed = false, .value = 0 };
}
