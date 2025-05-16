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
TriggerList * TriggerListSemanticAction(TriggerList * triggerList, Trigger * trigger);
StateList * StateListSemanticAction(StateList * stateList, State * state);
TransitionList * TransitionListSemanticAction(TransitionList * transitionList, Transition * transition);
State * StateDefinitionSemanticAction(const char * name, Style * style);
Transition * DirectionTransitionSemanticAction(const char * fromState,Direction  direction, const char * toState, Animate * animate);
Style * StyleSemanticAction(PropertyList * propertyList);
PropertyList * PropertyListSemanticAction(PropertyList * propertyList, Property * property);
Property * ValuePropertySemanticAction(const char * name, const char * value);
Property * FloatValuePropertySemanticAction(const char * name, const float value);
Property * IntegerValuePropertySemanticAction(const char * name, const int value);
Animate * AnimateSemanticAction(const char * time, const char * easing);


#endif
