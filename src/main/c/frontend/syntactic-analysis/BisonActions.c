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

Property * TextValuePropertySemanticAction(char * name, char * value) {
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

Animate * AnimateSemanticAction(AnimateInfo * animateInfo) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Animate * animate = calloc(1, sizeof(Animate));
    if (animate == NULL) {
        logError(_logger, "Memory allocation failed for Animate");
        return NULL;
    }
    animate->animateInfo = animateInfo;
    animate->type = NONE;
    return animate;
}

Animate * AnimateWithStyleSemanticAction( AnimateInfo * animateInfo, Style * style) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Animate * animate = calloc(1, sizeof(Animate));
    if (animate == NULL) {
        logError(_logger, "Memory allocation failed for Animate");
        return NULL;
    }
    animate->animateInfo = animateInfo;
    animate->style = style;
    animate->type = ANIMATE_WITH_STYLE;
    return animate;
}

Animate * AnimateWithKeyframesSemanticAction( AnimateInfo * animateInfo, Keyframes * keyframes) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Animate * animate = calloc(1, sizeof(Animate));
    if (animate == NULL) {
        logError(_logger, "Memory allocation failed for Animate");
        return NULL;
    }
    animate->animateInfo = animateInfo;
    animate->keyframes = keyframes;
    animate->type = ANIMATE_WITH_KEYFRAMES;
    return animate;
}

AnimateInfo * AnimateInfoSemanticAction(char * duration, char * delay, char * easing, AnimateInfoType type) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    AnimateInfo * animateInfo = calloc(1, sizeof(AnimateInfo));
    if (animateInfo == NULL) {
        logError(_logger, "Memory allocation failed for AnimateInfo");
        return NULL;
    }
    animateInfo->duration = duration;
    animateInfo->delay = delay;
    animateInfo->easing = easing;
    animateInfo->type = type;
    return animateInfo;
}

TransitionBlock * AnimateTransitionBlockSemanticAction(Animate * animate) { //TODO check and or simplify
    _logSyntacticAnalyzerAction(__FUNCTION__);
    TransitionBlock * transitionBlock = calloc(1, sizeof(TransitionBlock));
    if (transitionBlock == NULL) {
        logError(_logger, "Memory allocation failed for TransitionBlock");
        return NULL;
    }
    transitionBlock->stepItemList = calloc(1, sizeof(StepItemList));
    if (transitionBlock->stepItemList == NULL) {
        logError(_logger, "Memory allocation failed for StepItemList");
        free(transitionBlock);
        return NULL;
    }
    transitionBlock->stepItemList->items = calloc(1, sizeof(StepItem *));
    if (transitionBlock->stepItemList->items == NULL) {
        logError(_logger, "Memory allocation failed for StepItem");
        free(transitionBlock->stepItemList);
        free(transitionBlock);
        return NULL;
    }
    transitionBlock->stepItemList->items[0] = calloc(1, sizeof(StepItem));
    if (transitionBlock->stepItemList->items[0] == NULL) {
        logError(_logger, "Memory allocation failed for StepItem");
        free(transitionBlock->stepItemList->items);
        free(transitionBlock->stepItemList);
        free(transitionBlock);
        return NULL;
    }
    transitionBlock->stepItemList->items[0]->type = ANIMATE_ITEM;
    transitionBlock->stepItemList->items[0]->item = animate;
    transitionBlock->stepItemList->itemCount = 1;
    return transitionBlock;
}

TransitionBlock * StepItemListTransitionBlockSemanticAction(StepItemList * stepItemList) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    TransitionBlock * transitionBlock = calloc(1, sizeof(TransitionBlock));
    if (transitionBlock == NULL) {
        logError(_logger, "Memory allocation failed for TransitionBlock");
        return NULL;
    }
    transitionBlock->stepItemList = stepItemList;
    return transitionBlock;
}

StepItemList * StepItemListSemanticAction(StepItem * stepItem) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    StepItemList * stepItemList = calloc(1, sizeof(StepItemList));
    if (stepItemList == NULL) {
        logError(_logger, "Memory allocation failed for StepItemList");
        return NULL;
    }
    stepItemList->items = calloc(1, sizeof(StepItem *));
    if (stepItemList->items == NULL) {
        logError(_logger, "Memory allocation failed for StepItem");
        free(stepItemList);
        return NULL;
    }
    stepItemList->items[0] = stepItem;
    stepItemList->itemCount = 1;
    return stepItemList;
}


StepItemList * StepItemStepItemListSemanticAction(StepItemList * stepItemList, StepItem * stepItem) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    if (stepItemList == NULL) {
        logError(_logger, "Memory allocation failed for StepItemList");
        return NULL;
    }
    stepItemList->items = realloc(stepItemList->items, (stepItemList->itemCount + 1) * sizeof(StepItem *));
    if (stepItemList->items == NULL) {
        logError(_logger, "Memory allocation failed for StepItem");
        free(stepItemList);
        return NULL;
    }
    stepItemList->items[stepItemList->itemCount] = stepItem;
    stepItemList->itemCount++;
    return stepItemList;
}

StepItem * AnimateStepItemSemanticAction(Animate * animate) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    StepItem * stepItem = calloc(1, sizeof(StepItem));
    if (stepItem == NULL) {
        logError(_logger, "Memory allocation failed for StepItem");
        return NULL;
    }
    stepItem->type = ANIMATE_ITEM;
    stepItem->item = animate;
    return stepItem;
}
StepItem * StyleStepItemSemanticAction(Style * style) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    StepItem * stepItem = calloc(1, sizeof(StepItem));
    if (stepItem == NULL) {
        logError(_logger, "Memory allocation failed for StepItem");
        return NULL;
    }
    stepItem->type = STYLE_ITEM;
    stepItem->item = style;
    return stepItem;
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

StepItem * GroupStepItemSemanticAction(Group * group) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    StepItem * stepItem = calloc(1, sizeof(StepItem));
    if (stepItem == NULL) {
        logError(_logger, "Memory allocation failed for StepItem");
        return NULL;
    }
    stepItem->type = GROUP_ITEM;
    stepItem->item = group;
    return stepItem;
}

Keyframes * KeyframesSemanticAction(KeyframeStyleList * keyframeStyleList) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Keyframes * keyframes = calloc(1, sizeof(Keyframes));
    if (keyframes == NULL) {
        logError(_logger, "Memory allocation failed for Keyframes");
        return NULL;
    }
    keyframes->keyframeStyleList = keyframeStyleList;
    return keyframes;
}

KeyframeStyleList * KeyframeStyleKeyframeStyleListOffsetSemanticAction(KeyframeStyle * keyframeStyle) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    KeyframeStyleList * keyframeStyleList = calloc(1, sizeof(KeyframeStyleList));
    if (keyframeStyleList == NULL) {
        logError(_logger, "Memory allocation failed for KeyframeStyleList");
        return NULL;
    }
    keyframeStyleList->keyframeStyles = calloc(1, sizeof(KeyframeStyle *));
    if (keyframeStyleList->keyframeStyles == NULL) {
        logError(_logger, "Memory allocation failed for KeyframeStyle");
        free(keyframeStyleList);
        return NULL;
    }
    keyframeStyleList->keyframeStyles[0] = keyframeStyle;
    keyframeStyleList->keyframeCount = 1;
    return keyframeStyleList;
}

KeyframeStyleList * KeyframeStyleListSemanticAction(KeyframeStyleList * keyframeStyleList, KeyframeStyle * keyframeStyle) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    if (keyframeStyleList == NULL) {
        logError(_logger, "Memory allocation failed for KeyframeStyleList");
        return NULL;
    }
    keyframeStyleList->keyframeStyles = realloc(keyframeStyleList->keyframeStyles, (keyframeStyleList->keyframeCount + 1) * sizeof(KeyframeStyle *));
    if (keyframeStyleList->keyframeStyles == NULL) {
        logError(_logger, "Memory allocation failed for KeyframeStyle");
        free(keyframeStyleList);
        return NULL;
    }
    keyframeStyleList->keyframeStyles[keyframeStyleList->keyframeCount] = keyframeStyle;
    keyframeStyleList->keyframeCount++;
    return keyframeStyleList;
}

KeyframeStyle * keyframeStyleSemanticAction(PropertyList * propertyList, float offset) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    KeyframeStyle * keyframeStyle = calloc(1, sizeof(KeyframeStyle));
    if (keyframeStyle == NULL) {
        logError(_logger, "Memory allocation failed for KeyframeStyle");
        return NULL;
    }
    keyframeStyle->properties = propertyList;
    keyframeStyle->offset = offset;
    return keyframeStyle;
}

Group * GroupSemanticAction(StepItemList * stepItemList) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Group * group = calloc(1, sizeof(Group));

    if (group == NULL) {
        logError(_logger, "Memory allocation failed for Group");
        return NULL;
    }
    group->stepItemList = stepItemList;
    return group;
}

StepItem * SequenceStepItemSemanticAction(Sequence * sequence) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    StepItem * stepItem = calloc(1, sizeof(StepItem));
    if (stepItem == NULL) {
        logError(_logger, "Memory allocation failed for StepItem");
        return NULL;
    }
    stepItem->type = SEQUENCE_ITEM;
    stepItem->item = sequence;
    return stepItem;
}

Sequence * SequenceSemanticAction(StepItemList * stepItemList) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Sequence * sequence = calloc(1, sizeof(Sequence));
    if (sequence == NULL) {
        logError(_logger, "Memory allocation failed for Sequence");
        return NULL;
    }
    sequence->stepItemList = stepItemList;
    return sequence;
}

StepItem * StaggerStepItemSemanticAction(Stagger * stagger) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    StepItem * stepItem = calloc(1, sizeof(StepItem));
    if (stepItem == NULL) {
        logError(_logger, "Memory allocation failed for StepItem");
        return NULL;
    }
    stepItem->type = STAGGER_ITEM;
    stepItem->item = stagger;
    return stepItem;
}
Stagger * StaggerSemanticAction(char * time, StepItemList * stepItemList) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Stagger * stagger = calloc(1, sizeof(Stagger));
    if (stagger == NULL) {
        logError(_logger, "Memory allocation failed for Stagger");
        return NULL;
    }
    stagger->time = time;
    stagger->stepItemList = stepItemList;
    return stagger;
}

