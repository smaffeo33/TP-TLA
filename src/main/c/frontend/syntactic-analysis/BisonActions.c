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

Trigger * TriggerSemanticAction(char * name, TriggerBlock * triggerBlock) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Trigger * trigger = calloc(1, sizeof(Trigger));
    if (trigger == NULL) {
        logError(_logger, "Memory allocation failed for Trigger");
        return NULL;
    }
    trigger->name = name;
    trigger->block = triggerBlock;
    return trigger;
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

TriggerList * TriggerTriggerListSemanticAction(Trigger * trigger) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    TriggerList * triggerList = calloc(1, sizeof(TriggerList));
    if (triggerList == NULL) {
        logError(_logger, "Memory allocation failed for TriggerList");
        return NULL;
    }
    triggerList->trigger = calloc(1, sizeof(Trigger *));
    if (triggerList->trigger == NULL) {
        logError(_logger, "Memory allocation failed for Trigger");
        free(triggerList);
        return NULL;
    }
    triggerList->trigger[0] = trigger;
    triggerList->triggerCount = 1;
    return triggerList;
}

StateList * StateStateListSemanticAction(State * state) {
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

TransitionList * TransitionTransitionListSemanticAction(Transition * transition) {
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

State * StateDefinitionSemanticAction( char * name, Style * style) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    State * state = calloc(1, sizeof(State));
    if (state == NULL) {
        logError(_logger, "Memory allocation failed for State");
        return NULL;
    }
    state->name = name;
    state->style = style;
    return state;
}


Transition * TransitionSemanticAction( TransitionRule * transitionRule, TransitionBlock * transitionBlock) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Transition * transition = calloc(1, sizeof(Transition));
    if (transition == NULL) {
        logError(_logger, "Memory allocation failed for Transition");
        return NULL;
    }
    transition->transitionRule = transitionRule;
    transition->transitionBlock = transitionBlock;
    return transition;
}


TriggerBlock * StatelessTriggerBlockSemanticAction(TransitionList * transitionList) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    TriggerBlock * triggerBlock = calloc(1, sizeof(TriggerBlock));
    if (triggerBlock == NULL) {
        logError(_logger, "Memory allocation failed for TriggerBlock");
        return NULL;
    }
    triggerBlock->stateList = NULL;
    triggerBlock->transitionList = transitionList;
    return triggerBlock;
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

PropertyList * PropertyPropertyListSemanticAction(Property * property) {
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

Property * ValuePropertySemanticAction( char * name,  char * value) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Property * property = calloc(1, sizeof(Property));
    if (property == NULL) {
        logError(_logger, "Memory allocation failed for Property");
        return NULL;
    }
    property->name = name;
    property->value = value;
    property->type = STRING;
    return property;
}

Property * FloatValuePropertySemanticAction( char * name,  float value) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Property * property = calloc(1, sizeof(Property));
    if (property == NULL) {
        logError(_logger, "Memory allocation failed for Property");
        return NULL;
    }
    property->name = name;
    property->floatValue = value;
    property->type = FLOAT;
    return property;
}

Property * IntegerValuePropertySemanticAction( char * name,  int value) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Property * property = calloc(1, sizeof(Property));
    if (property == NULL) {
        logError(_logger, "Memory allocation failed for Property");
        return NULL;
    }
    property->name = name;
    property->intValue = value;
    property->type = INT;
    return property;
}

Animate * AnimateSemanticAction( char * time,  char * easing) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Animate * animate = calloc(1, sizeof(Animate));
    if (animate == NULL) {
        logError(_logger, "Memory allocation failed for Animate");
        return NULL;
    }
    animate->duration = time;
    animate->easing = easing;
    animate->type = NONE;
    return animate;
}

Animate * AnimateWithStyleSemanticAction( char * time,  char * easing, Style * style) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Animate * animate = calloc(1, sizeof(Animate));
    if (animate == NULL) {
        logError(_logger, "Memory allocation failed for Animate");
        return NULL;
    }
    animate->duration = time;
    animate->easing = easing;
    animate->style = style;
    animate->type = ANIMATE_WITH_STYLE;
    return animate;
}

Animate * AnimateWithKeyframesSemanticAction( char * time,  char * easing, Keyframes * keyframes) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Animate * animate = calloc(1, sizeof(Animate));
    if (animate == NULL) {
        logError(_logger, "Memory allocation failed for Animate");
        return NULL;
    }
    animate->duration = time;
    animate->easing = easing;
    animate->keyframes = keyframes;
    animate->type = ANIMATE_WITH_KEYFRAMES;
    return animate;
}

TransitionBlock * AnimateTransitionBlockSemanticAction(Animate * animate) { //TODO check and or simplify
    _logSyntacticAnalyzerAction(__FUNCTION__);
    TransitionBlock * transitionBlock = calloc(1, sizeof(TransitionBlock));
    if (transitionBlock == NULL) {
        logError(_logger, "Memory allocation failed for TransitionBlock");
        return NULL;
    }
    transitionBlock->transitionBlockItemList = calloc(1, sizeof(TransitionBlockItemList));
    if (transitionBlock->transitionBlockItemList == NULL) {
        logError(_logger, "Memory allocation failed for TransitionBlockItemList");
        free(transitionBlock);
        return NULL;
    }
    transitionBlock->transitionBlockItemList->items = calloc(1, sizeof(TransitionBlockItem *));
    if (transitionBlock->transitionBlockItemList->items == NULL) {
        logError(_logger, "Memory allocation failed for TransitionBlockItem");
        free(transitionBlock->transitionBlockItemList);
        free(transitionBlock);
        return NULL;
    }
    transitionBlock->transitionBlockItemList->items[0] = calloc(1, sizeof(TransitionBlockItem));
    if (transitionBlock->transitionBlockItemList->items[0] == NULL) {
        logError(_logger, "Memory allocation failed for TransitionBlockItem");
        free(transitionBlock->transitionBlockItemList->items);
        free(transitionBlock->transitionBlockItemList);
        free(transitionBlock);
        return NULL;
    }
    transitionBlock->transitionBlockItemList->items[0]->type = ANIMATE_ITEM;
    transitionBlock->transitionBlockItemList->items[0]->item = animate;
    transitionBlock->transitionBlockItemList->itemCount = 1;
    return transitionBlock;
}

TransitionBlock * TransitionBlockItemListTransitionBlockSemanticAction(TransitionBlockItemList * transitionBlockItemList) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    TransitionBlock * transitionBlock = calloc(1, sizeof(TransitionBlock));
    if (transitionBlock == NULL) {
        logError(_logger, "Memory allocation failed for TransitionBlock");
        return NULL;
    }
    transitionBlock->transitionBlockItemList = transitionBlockItemList;
    return transitionBlock;
}

TransitionBlockItemList * TransitionBlockItemListSemanticAction(TransitionBlockItem * transitionBlockItem) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    TransitionBlockItemList * transitionBlockItemList = calloc(1, sizeof(TransitionBlockItemList));
    if (transitionBlockItemList == NULL) {
        logError(_logger, "Memory allocation failed for TransitionBlockItemList");
        return NULL;
    }
    transitionBlockItemList->items = calloc(1, sizeof(TransitionBlockItem *));
    if (transitionBlockItemList->items == NULL) {
        logError(_logger, "Memory allocation failed for TransitionBlockItem");
        free(transitionBlockItemList);
        return NULL;
    }
    transitionBlockItemList->items[0] = transitionBlockItem;
    transitionBlockItemList->itemCount = 1;
    return transitionBlockItemList;
}
TransitionBlockItemList * AnimateTransitionBlockItemListSemanticAction(TransitionBlockItemList * transitionBlockItemList, Animate * animate) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    TransitionBlockItem * transitionBlockItem = calloc(1, sizeof(TransitionBlockItem));
    if (transitionBlockItem == NULL) {
        logError(_logger, "Memory allocation failed for TransitionBlockItem");
        return NULL;
    }
    transitionBlockItem->type = ANIMATE_ITEM;
    transitionBlockItem->item = animate;
    transitionBlockItemList->items = realloc(transitionBlockItemList->items, (transitionBlockItemList->itemCount + 1) * sizeof(TransitionBlockItem *));
    if (transitionBlockItemList->items == NULL) {
        logError(_logger, "Memory allocation failed for TransitionBlockItem");
        free(transitionBlockItem);
        return NULL;
    }
    transitionBlockItemList->items[transitionBlockItemList->itemCount] = transitionBlockItem;
    transitionBlockItemList->itemCount++;
    return transitionBlockItemList;
}
TransitionBlockItemList * StyleTransitionBlockItemListSemanticAction(TransitionBlockItemList * transitionBlockItemList, Style * style) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    TransitionBlockItem * transitionBlockItem = calloc(1, sizeof(TransitionBlockItem));
    if (transitionBlockItem == NULL) {
        logError(_logger, "Memory allocation failed for TransitionBlockItem");
        return NULL;
    }
    transitionBlockItem->type = STYLE_ITEM;
    transitionBlockItem->item = style;
    transitionBlockItemList->items = realloc(transitionBlockItemList->items, (transitionBlockItemList->itemCount + 1) * sizeof(TransitionBlockItem *));
    if (transitionBlockItemList->items == NULL) {
        logError(_logger, "Memory allocation failed for TransitionBlockItem");
        free(transitionBlockItem);
        return NULL;
    }
    transitionBlockItemList->items[transitionBlockItemList->itemCount] = transitionBlockItem;
    transitionBlockItemList->itemCount++;
    return transitionBlockItemList;
}

TransitionBlockItem * AnimateTransitionBlockItemSemanticAction(Animate * animate) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    TransitionBlockItem * transitionBlockItem = calloc(1, sizeof(TransitionBlockItem));
    if (transitionBlockItem == NULL) {
        logError(_logger, "Memory allocation failed for TransitionBlockItem");
        return NULL;
    }
    transitionBlockItem->type = ANIMATE_ITEM;
    transitionBlockItem->item = animate;
    return transitionBlockItem;
}
TransitionBlockItem * StyleTransitionBlockItemSemanticAction(Style * style) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    TransitionBlockItem * transitionBlockItem = calloc(1, sizeof(TransitionBlockItem));
    if (transitionBlockItem == NULL) {
        logError(_logger, "Memory allocation failed for TransitionBlockItem");
        return NULL;
    }
    transitionBlockItem->type = STYLE_ITEM;
    transitionBlockItem->item = style;
    return transitionBlockItem;
}

TransitionRule * TransitionRuleSemanticAction(char * fromState, char * toState, Direction direction) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    TransitionRule * transitionRule = calloc(1, sizeof(TransitionRule));
    if (transitionRule == NULL) {
        logError(_logger, "Memory allocation failed for TransitionRule");
        return NULL;
    }
    transitionRule->fromState = fromState;
    transitionRule->toState = toState;
    transitionRule->direction = direction;
    transitionRule->ruleType = FROM_TO;
    return transitionRule;
}

TransitionRule * AliasTypeSemanticAction(AliasType aliasType) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    TransitionRule * transitionRule = calloc(1, sizeof(TransitionRule));
    if (transitionRule == NULL) {
        logError(_logger, "Memory allocation failed for TransitionRule");
        return NULL;
    }
    transitionRule->alias = aliasType;
    transitionRule->ruleType = ALIAS;
    return transitionRule;
}
AliasType AliasSemanticAction(AliasType aliasType) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    return aliasType;
}

TransitionBlockItem * GroupTransitionBlockItemSemanticAction(Group * group) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    TransitionBlockItem * transitionBlockItem = calloc(1, sizeof(TransitionBlockItem));
    if (transitionBlockItem == NULL) {
        logError(_logger, "Memory allocation failed for TransitionBlockItem");
        return NULL;
    }
    transitionBlockItem->type = GROUP_ITEM;
    transitionBlockItem->item = group;
    return transitionBlockItem;
}

Keyframes * KeyframesSemanticAction(StyleList * styleList) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Keyframes * keyframes = calloc(1, sizeof(Keyframes));
    if (keyframes == NULL) {
        logError(_logger, "Memory allocation failed for Keyframes");
        return NULL;
    }
    keyframes->styleList = styleList;
    return keyframes;
}

StyleList * StyleListSemanticAction(StyleList * styleList, Style * style) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    if (styleList == NULL) {
        logError(_logger, "Memory allocation failed for StyleList");
        return NULL;
    }
    styleList->styles = realloc(styleList->styles, (styleList->styleCount + 1) * sizeof(Style *));
    if (styleList->styles == NULL) {
        logError(_logger, "Memory allocation failed for Style");
        free(styleList);
        return NULL;
    }
    styleList->styles[styleList->styleCount] = style;
    styleList->styleCount++;
    return styleList;
}

StyleList * StyleStyleListSemanticAction(Style * style) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    StyleList * styleList = calloc(1, sizeof(StyleList));
    if (styleList == NULL) {
        logError(_logger, "Memory allocation failed for StyleList");
        return NULL;
    }
    styleList->styles = calloc(1, sizeof(Style *));
    if (styleList->styles == NULL) {
        logError(_logger, "Memory allocation failed for Style");
        free(styleList);
        return NULL;
    }
    styleList->styles[0] = style;
    styleList->styleCount = 1;
    return styleList;
}

Group * GroupSemanticAction(TransitionBlockItemList * transitionBlockItemList) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Group * group = calloc(1, sizeof(Group));

    if (group == NULL) {
        logError(_logger, "Memory allocation failed for Group");
        return NULL;
    }
    group->transitionBlockItemList = transitionBlockItemList;
    return group;
}

