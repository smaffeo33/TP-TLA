#include "BisonActions.h"

/* MODULE INTERNAL STATE */

static Logger * _logger = NULL;

void initializeBisonActionsModule() {
	_logger = createLogger("BisonActions");
}

void shutdownBisonActionsModule() {
	if (_logger != NULL) {
		destroyLogger(_logger);
	}
}

/** IMPORTED FUNCTIONS */

extern unsigned int flexCurrentContext(void);

/* PRIVATE FUNCTIONS */

static void _logSyntacticAnalyzerAction(const char * functionName);

/**
 * Logs a syntactic-analyzer action in DEBUGGING level.
 */
static void _logSyntacticAnalyzerAction(const char * functionName) {
	logDebugging(_logger, "%s", functionName);
}

/* PUBLIC FUNCTIONS */
Program * TriggerListProgramSemanticAction(CompilerState * compilerState, TriggerList * triggerList) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Program * program = calloc(1, sizeof(Program));
    program->triggerList = triggerList;
    compilerState->abstractSyntaxtTree = program;
    if (0 < flexCurrentContext()) {
        logError(_logger, "The final context is not the default (0): %d", flexCurrentContext());
        compilerState->succeed = false;
    }
    else {
        compilerState->succeed = true;
    }
    return program;
}

TriggerList * TriggerSemanticAction(char *name, Trigger * trigger) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    TriggerList * triggerList = calloc(1, sizeof(TriggerList));
    if(triggerList == NULL) {
        logError(_logger, "Memory allocation failed for TriggerList");
        return NULL;
    }
    triggerList->trigger = calloc(1, sizeof(Trigger *));
    if(triggerList->trigger == NULL) {
        logError(_logger, "Memory allocation failed for Trigger");
        free(triggerList);
        return NULL;
    }
    triggerList->trigger[0] = trigger;
    triggerList->triggerCount = 1;
    return triggerList;
}

TriggerList * TriggerListSemanticAction(TriggerList * triggerList, Trigger * trigger) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    if (triggerList == NULL) {
        logError(_logger, "Memory allocation failed for TriggerList");
        return NULL;
    }
    triggerList->trigger = realloc(triggerList->trigger, (triggerList->triggerCount + 1) * sizeof(Trigger *));
    if(triggerList->trigger == NULL) {
        logError(_logger, "Memory allocation failed for Trigger");
        free(triggerList);
        return NULL;
    }
    triggerList->trigger[triggerList->triggerCount] = trigger;
    triggerList->triggerCount++;
    return triggerList;
}
TriggerBlock * TriggerBlockSemanticAction(StateList * stateList, TransitionList * transitionList) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    TriggerBlock * triggerBlock = calloc(1, sizeof(TriggerBlock));
    if (triggerBlock == NULL) {
        logError(_logger, "Memory allocation failed for TriggerBlock");
        return NULL;
    }
    triggerBlock->stateList = stateList;
    triggerBlock->transitionList = transitionList;
    return triggerBlock;
}

StateList * StateSemanticAction(State * state) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    StateList * stateList = calloc(1, sizeof(StateList));
    if (stateList == NULL) {
        logError(_logger, "Memory allocation failed for StateList");
        return NULL;
    }
    stateList->states = calloc(1, sizeof(State *));
    if (stateList->states == NULL) {
        logError(_logger, "Memory allocation failed for State");
        free(stateList);
        return NULL;
    }
    stateList->states[0] = state;
    stateList->stateCount = 1;
    return stateList;
}
StateList * StateListSemanticAction(StateList * stateList, State * state) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    if (stateList == NULL) {
        logError(_logger, "Memory allocation failed for StateList");
        return NULL;
    }
    stateList->states = realloc(stateList->states, (stateList->stateCount + 1) * sizeof(State *));
    if (stateList->states == NULL) {
        logError(_logger, "Memory allocation failed for State");
        free(stateList);
        return NULL;
    }
    stateList->states[stateList->stateCount] = state;
    stateList->stateCount++;
    return stateList;
}

TransitionList * TransitionSemanticAction(Transition * transition) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    TransitionList * transitionList = calloc(1, sizeof(TransitionList));
    if (transitionList == NULL) {
        logError(_logger, "Memory allocation failed for TransitionList");
        return NULL;
    }
    transitionList->transitions = calloc(1, sizeof(Transition *));
    if (transitionList->transitions == NULL) {
        logError(_logger, "Memory allocation failed for Transition");
        free(transitionList);
        return NULL;
    }
    transitionList->transitions[0] = transition;
    transitionList->transitionCount = 1;
    return transitionList;
}

TransitionList * TransitionListSemanticAction(TransitionList * transitionList, Transition * transition) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    if (transitionList == NULL) {
        logError(_logger, "Memory allocation failed for TransitionList");
        return NULL;
    }
    transitionList->transitions = realloc(transitionList->transitions, (transitionList->transitionCount + 1) * sizeof(Transition *));
    if (transitionList->transitions == NULL) {
        logError(_logger, "Memory allocation failed for Transition");
        free(transitionList);
        return NULL;
    }
    transitionList->transitions[transitionList->transitionCount] = transition;
    transitionList->transitionCount++;
    return transitionList;
}

State * StateDefinitionSemanticAction(const char * name, Style * style) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    State * state = calloc(1, sizeof(State));
    if (state == NULL) {
        logError(_logger, "Memory allocation failed for State");
        return NULL;
    }
    state->name = strdup(name);
    state->style = style;
    return state;
}

Transition  * DirectionalTransitionSemanticAction(const char * fromState, Direction direction, const char * toState, Animate * animate) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Transition * transition = calloc(1, sizeof(Transition));
    if (transition == NULL) {
        logError(_logger, "Memory allocation failed for Transition");
        return NULL;
    }
    transition->fromState = strdup(fromState);
    transition->toState = strdup(toState);
    transition->direction = direction;
    transition->animate = animate;
    return transition;
}



Style * StyleSemanticAction(PropertyList * propertyList) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Style * style = calloc(1, sizeof(Style));
    if (style == NULL) {
        logError(_logger, "Memory allocation failed for Style");
        return NULL;
    }
    style->properties = propertyList;
    return style;
}

PropertyList * PropertySemanticAction(Property * property) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    PropertyList * propertyList = calloc(1, sizeof(PropertyList));
    if (propertyList == NULL) {
        logError(_logger, "Memory allocation failed for PropertyList");
        return NULL;
    }
    propertyList->properties = calloc(1, sizeof(Property *));
    if (propertyList->properties == NULL) {
        logError(_logger, "Memory allocation failed for Property");
        free(propertyList);
        return NULL;
    }
    propertyList->properties[0] = property;
    propertyList->propertyCount = 1;
    return propertyList;
}

PropertyList * PropertyListSemanticAction(PropertyList * propertyList, Property * property) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    if (propertyList == NULL) {
        logError(_logger, "Memory allocation failed for PropertyList");
        return NULL;
    }
    propertyList->properties = realloc(propertyList->properties, (propertyList->propertyCount + 1) * sizeof(Property *));
    if (propertyList->properties == NULL) {
        logError(_logger, "Memory allocation failed for Property");
        free(propertyList);
        return NULL;
    }
    propertyList->properties[propertyList->propertyCount] = property;
    propertyList->propertyCount++;
    return propertyList;
}

Property * ValuePropertySemanticAction(const char * name, const char * value) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Property * property = calloc(1, sizeof(Property));
    if (property == NULL) {
        logError(_logger, "Memory allocation failed for Property");
        return NULL;
    }
    property->name = strdup(name);
    property->value = strdup(value);
    return property;
}

Animate * AnimateSemanticAction(const char * time, const char * easing) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Animate * animate = calloc(1, sizeof(Animate));
    if (animate == NULL) {
        logError(_logger, "Memory allocation failed for Animate");
        return NULL;
    }
    animate->duration = strdup(time);
    animate->easing = strdup(easing);
    return animate;
}

