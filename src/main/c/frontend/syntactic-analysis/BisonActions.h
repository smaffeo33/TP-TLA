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
Animate * AnimateSemanticAction( char * time,  char * easing);
Animate * AnimateWithStyleSemanticAction( char * time,  char * easing, Style * style);
Animate * AnimateWithKeyframesSemanticAction( char * time,  char * easing, Keyframes * keyframes);
TransitionBlock * AnimateTransitionBlockSemanticAction(Animate * animate);
TransitionRule * TransitionRuleSemanticAction(char * fromState, char * toState, Direction direction);

TransitionBlock * TransitionBlockItemListTransitionBlockSemanticAction(TransitionBlockItemList * transitionBlockItemList);

TransitionBlockItemList * TransitionBlockItemListSemanticAction(TransitionBlockItem * transitionBlockItem);
TransitionBlockItemList * TransitionBlockItemTransitionBlockItemListSemanticAction(TransitionBlockItemList * transitionBlockItemList, TransitionBlockItem * transitionBlockItem);

TransitionBlockItem * AnimateTransitionBlockItemSemanticAction(Animate * animate);
TransitionBlockItem * StyleTransitionBlockItemSemanticAction(Style * style);

Keyframes * KeyframesSemanticAction(KeyframeStyleList * keyframeStyleList);

TransitionRule * AliasTypeSemanticAction(AliasType aliasType);
AliasType AliasSemanticAction(AliasType aliasType);

TransitionBlockItem * GroupTransitionBlockItemSemanticAction(Group * group);

Group * GroupSemanticAction(TransitionBlockItemList * transitionBlockItemList);

TransitionBlockItem * SequenceTransitionBlockItemSemanticAction(Sequence * sequence);
Sequence * SequenceSemanticAction(TransitionBlockItemList * transitionBlockItemList);

TransitionBlockItem * StaggerTransitionBlockItemSemanticAction(Stagger * stagger);
Stagger * StaggerSemanticAction(char * time, TransitionBlockItemList * transitionBlockItemList);

KeyframeStyleList * KeyframStyleKeyframeStyleListOffsetSemanticAction(KeyframeStyle * keyframeStyle);
KeyframeStyleList * KeyframeStyleListSemanticAction(KeyframeStyleList * keyframeStyleList, KeyframeStyle * keyframeStyle);

KeyframeStyle * keframeStyleSemanticAction(PropertyList * propertyList, float offset);


#endif
