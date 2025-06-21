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
static boolean _validateStyle(Style *style, Trigger *parent, SymbolTable *globalSymbols);


#define MAX_DURATION_MS 60000
#define MAX_DURATION_S  60.0

ComputationResult validateProgram(Program *program, SymbolTable *globalSymbols)
{
    if (!program || !program->triggerList || program->triggerList->triggerCount == 0) {
        logError(_logger, "Invalid or empty program.");
        return _invalidComputation();
    }


    size_t errorCount = 0;



    for (size_t i = 0; i < program->triggerList->triggerCount; ++i) {
        Trigger *t = program->triggerList->trigger[i];
        if (!t) {
            logError(_logger, "Null trigger at index %zu", i);
            ++errorCount;
            continue;
        }


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

    stAdd(symbols, "*", SYMBOL_STATE);
    stAdd(symbols, "void", SYMBOL_STATE);

    size_t errorCount = 0;

    // ✅ Safeguard stateList
    if (t->block->stateList) {
        for (size_t i = 0; i < t->block->stateList->stateCount; ++i) {
            if (!_validateState(t, t->block->stateList->states[i], symbols))
                ++errorCount;
        }
    }

    // ✅ Also safeguard transitionList
    if (t->block->transitionList) {
        for (size_t i = 0; i < t->block->transitionList->transitionCount; ++i) {
            if (!_validateTrans(t, t->block->transitionList->transitions[i], symbols, globalSymbols))
                ++errorCount;
        }
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
        case STYLE_ITEM:            return _validateStyle(si->item, parent, globalSymbols);
        case QUERY_ITEM:          return _validateQuery(si->item, parent, globalSymbols);
        case GROUP_ITEM:          return _validateGroup(si->item, parent, globalSymbols);
        case SEQUENCE_ITEM:       return _validateSequence(si->item, parent, globalSymbols);
        case STAGGER_ITEM:        return _validateStagger(si->item, parent, globalSymbols);
        case ANIMATE_CHILD_ITEM:  return _validateAnimateChild(si->item, parent, globalSymbols); // si tenés uno
        default:
            logError(_logger, "Unknown step type in trigger '%s'. IT IS: %d", parent->name,
                     (si->type));
            return false;
    }
}

static boolean _validateAnimate(Animate *a, Trigger *parent) {
    if (!a || !a->animateInfo) return false;
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
    if (si || !parent) {
        logError(_logger, "animateChild() called in invalid context.");
        return false;
    }


    Symbol *triggerSym = stGet(globalSymbols, parent->name);
    if (!triggerSym || !triggerSym->subTable) {
        logError(_logger, "Trigger '%s' has no subTable; animateChild() might not be valid.", parent->name);
    }

    // Como no se puede verificar hijos explícitamente, acepta siempre.
    return true;
}

// Declaro un helper para validar strings válidos según la propiedad
static boolean _isValidStringValueForProperty(const char *propertyName, const char *value) {
    if (!value) return false;

    // Arrays con valores válidos por propiedad
    static const char *displayValues[] = {"none", "block", "inline", "inline-block", "flex", "grid", "inline-flex"};
    static const char *positionValues[] = {"static", "relative", "absolute", "fixed", "sticky"};
    static const char *overflowValues[] = {"visible", "hidden", "scroll", "auto"};
    static const char *visibilityValues[] = {"visible", "hidden", "collapse"};
    static const char *textAlignValues[] = {"left", "right", "center", "justify", "start", "end"};
    static const char *whiteSpaceValues[] = {"normal", "nowrap", "pre", "pre-wrap", "pre-line"};
    static const char *fontStyleValues[] = {"normal", "italic", "oblique"};
    static const char *textDecorationValues[] = {"none", "underline", "overline", "line-through", "blink"};
    static const char *textTransformValues[] = {"none", "capitalize", "uppercase", "lowercase", "full-width"};
    static const char *flexDirectionValues[] = {"row", "row-reverse", "column", "column-reverse"};
    static const char *justifyContentValues[] = {"flex-start", "flex-end", "center", "space-between", "space-around", "space-evenly"};
    static const char *alignItemsValues[] = {"stretch", "center", "flex-start", "flex-end", "baseline"};

    #define CHECK_VALUE(arr) do { \
        for (size_t i = 0; i < sizeof(arr)/sizeof(arr[0]); ++i) { \
            if (strcmp(value, arr[i]) == 0) return true; \
        } \
    } while(0)

    if (strcmp(propertyName, "display") == 0) {
        CHECK_VALUE(displayValues);
        return false;
    }
    if (strcmp(propertyName, "position") == 0) {
        CHECK_VALUE(positionValues);
        return false;
    }
    if (strcmp(propertyName, "overflow") == 0 ||
        strcmp(propertyName, "overflow-x") == 0 ||
        strcmp(propertyName, "overflow-y") == 0) {
        CHECK_VALUE(overflowValues);
        return false;
    }
    if (strcmp(propertyName, "visibility") == 0) {
        CHECK_VALUE(visibilityValues);
        return false;
    }
    if (strcmp(propertyName, "text-align") == 0) {
        CHECK_VALUE(textAlignValues);
        return false;
    }
    if (strcmp(propertyName, "white-space") == 0) {
        CHECK_VALUE(whiteSpaceValues);
        return false;
    }
    if (strcmp(propertyName, "font-style") == 0) {
        CHECK_VALUE(fontStyleValues);
        return false;
    }
    if (strcmp(propertyName, "text-decoration") == 0) {
        CHECK_VALUE(textDecorationValues);
        return false;
    }
    if (strcmp(propertyName, "text-transform") == 0) {
        CHECK_VALUE(textTransformValues);
        return false;
    }
    if (strcmp(propertyName, "flex-direction") == 0) {
        CHECK_VALUE(flexDirectionValues);
        return false;
    }
    if (strcmp(propertyName, "justify-content") == 0) {
        CHECK_VALUE(justifyContentValues);
        return false;
    }
    if (strcmp(propertyName, "align-items") == 0) {
        CHECK_VALUE(alignItemsValues);
        return false;
    }

    // Para "transform" dejamos pasar cualquier string, porque puede ser complejo, se valida en el front
    if (strcmp(propertyName, "transform") == 0) {
        return strlen(value) > 0; // solo validamos que no sea vacío
    }

    return false; // propiedad desconocida o valor no válido
}

static boolean _isValidUnitValue(const char *value) {
    if (!value) return false;

    // Buscamos números al inicio
    size_t i = 0;
    boolean digitFound = false;
    while (value[i] && (isdigit(value[i]) || value[i] == '.' || value[i] == '-')) {
        if (isdigit(value[i])) digitFound = true;
        i++;
    }
    if (!digitFound) return false;

    const char *unit = value + i;
    // Unidades CSS más comunes
    if (strcmp(unit, "px") == 0 ||
        strcmp(unit, "em") == 0 ||
        strcmp(unit, "%") == 0 ||
        strcmp(unit, "rem") == 0 ||
        strcmp(unit, "vh") == 0 ||
        strcmp(unit, "vw") == 0) {
        return true;
    }
    return false;
}


static boolean _validateStyle(Style *style, Trigger *parent, SymbolTable *globalSymbols) {
    if (!style || !style->properties || style->properties->propertyCount == 0) {
        logError(_logger, "Empty style block in trigger '%s'.", parent->name);
        return false;
    }

    boolean allValid = true;

    for (size_t i = 0; i < style->properties->propertyCount; ++i) {
        Property *p = style->properties->properties[i];
        if (!p || !p->name) {
            logError(_logger, "Invalid property in style block of trigger '%s'.", parent->name);
            allValid = false;
            continue;
        }

        switch (p->type) {
            case STRING:
                if (!_isValidStringValueForProperty(p->name, p->value)) {
                    logError(_logger, "Invalid string value '%s' for property '%s' in trigger '%s'.",
                             p->value ? p->value : "(null)", p->name, parent->name);
                    allValid = false;
                }
                break;

            case UNIT:
                if (!_isValidUnitValue(p->value)) {
                    logError(_logger, "Invalid unit value '%s' for property '%s' in trigger '%s'.",
                             p->value ? p->value : "(null)", p->name, parent->name);
                    allValid = false;
                }
                break;

            case COLOR:
                if (!p->value || strlen(p->value) == 0) {
                    logError(_logger, "Empty color value for property '%s' in trigger '%s'.", p->name, parent->name);
                    allValid = false;
                }
                break;

            case UNITLESS:
                if (p->floatValue < 0) {
                    logError(_logger, "Negative unitless value '%f' for property '%s' in trigger '%s'.",
                             p->floatValue, p->name, parent->name);
                    allValid = false;
                }
                break;

            default:
                logError(_logger, "Unknown property type for '%s' in trigger '%s'.", p->name, parent->name);
                allValid = false;
                break;
        }
    }

    return allValid;
}


static ComputationResult _invalidComputation(void)
{
    return (ComputationResult){ .succeed = false, .value = 0 };
}
