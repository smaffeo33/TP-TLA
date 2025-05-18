#ifndef BISON_ACTIONS_HEADER
#define BISON_ACTIONS_HEADER

#include "../../shared/CompilerState.h"
#include "../../shared/Logger.h"
#include "../../shared/Type.h"
#include "AbstractSyntaxTree.h"
#include "SyntacticAnalyzer.h"
#include <stdlib.h>

/** Initialize module's internal state. */
void initializeBisonActionsModule();

/** Shutdown module's internal state. */
void shutdownBisonActionsModule();

/**
 * Bison semantic actions.
 */

Program * TriggerListProgramSemanticAction(CompilerState * compilerState, TriggerList * triggerList);
Trigger * TriggerSemanticAction(char *name, TriggerBlock * triggerBlock);

TriggerList * TriggerTriggerListSemanticAction(Trigger * trigger);
StateList * StateStateListSemanticAction(State * state);
TransitionList * TransitionTransitionListSemanticAction(Transition * transition);
PropertyList * PropertyPropertyListSemanticAction(Property * property);

TriggerBlock * TriggerBlockSemanticAction(StateList * stateList, TransitionList * transitionList);
TriggerBlock * StatelessTriggerBlockSemanticAction(TransitionList * transitionList);
TriggerList * TriggerListSemanticAction(TriggerList * triggerList, Trigger * trigger);
StateList * StateListSemanticAction(StateList * stateList, State * state);
TransitionList * TransitionListSemanticAction(TransitionList * transitionList, Transition * transition);
State * StateDefinitionSemanticAction( char * name, Style * style);
Transition * TransitionSemanticAction( TransitionRule * transitionRule, TransitionBlock * transitionBlock);
Style * StyleSemanticAction(PropertyList * propertyList);
PropertyList * PropertyListSemanticAction(PropertyList * propertyList, Property * property);
Property * ValuePropertySemanticAction( char * name,  char * value);
Property * FloatValuePropertySemanticAction( char * name,  float value);
Property * IntegerValuePropertySemanticAction( char * name,  int value);

Animate * AnimateSemanticAction(AnimateInfo * animateInfo);
Animate * AnimateWithStyleSemanticAction( AnimateInfo * animateInfo, Style * style);
Animate * AnimateWithKeyframesSemanticAction( AnimateInfo * animateInfo, Keyframes * keyframes);

AnimateInfo * AnimateInfoSemanticAction(char * duration, char * delay, char * easing, AnimateInfoType type);

TransitionBlock * AnimateTransitionBlockSemanticAction(Animate * animate);
TransitionRule * TransitionRuleSemanticAction(char * fromState, char * toState, Direction direction);

TransitionBlock * StepItemListTransitionBlockSemanticAction(StepItemList * stepItemList);

StepItemList * StepItemListSemanticAction(StepItem * stepItem);
StepItemList * StepItemStepItemListSemanticAction(StepItemList * stepItemList, StepItem * stepItem);

StepItem * AnimateStepItemSemanticAction(Animate * animate);
StepItem * StyleStepItemSemanticAction(Style * style);

Keyframes * KeyframesSemanticAction(KeyframeStyleList * keyframeStyleList);

TransitionRule * AliasTypeSemanticAction(AliasType aliasType);
AliasType AliasSemanticAction(AliasType aliasType);

StepItem * GroupStepItemSemanticAction(Group * group);

Group * GroupSemanticAction(StepItemList * stepItemList);

StepItem * SequenceStepItemSemanticAction(Sequence * sequence);
Sequence * SequenceSemanticAction(StepItemList * stepItemList);

StepItem * StaggerStepItemSemanticAction(Stagger * stagger);
Stagger * StaggerSemanticAction(char * time, StepItemList * stepItemList);

KeyframeStyleList * KeyframStyleKeyframeStyleListOffsetSemanticAction(KeyframeStyle * keyframeStyle);
KeyframeStyleList * KeyframeStyleListSemanticAction(KeyframeStyleList * keyframeStyleList, KeyframeStyle * keyframeStyle);

KeyframeStyle * keframeStyleSemanticAction(PropertyList * propertyList, float offset);


#endif
