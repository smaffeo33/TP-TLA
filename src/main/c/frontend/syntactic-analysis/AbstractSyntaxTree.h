#ifndef ABSTRACT_SYNTAX_TREE_HEADER
#define ABSTRACT_SYNTAX_TREE_HEADER

#include "../../shared/Logger.h"
#include <stdlib.h>

/** Initialize module's internal state. */
void initializeAbstractSyntaxTreeModule();

/** Shutdown module's internal state. */
void shutdownAbstractSyntaxTreeModule();

/**
 * This typedefs allows self-referencing types.
 */

typedef struct Program Program;
typedef struct Trigger Trigger;
typedef struct State State;
typedef struct Transition Transition;
typedef struct Style Style;
typedef struct Animate Animate;
typedef struct Property Property;
typedef struct TriggerList TriggerList;
typedef struct TriggerBlock TriggerBlock;

struct Trigger {
	char *name; // e.g., "cardAnimation"
	TriggerBlock *block;
};

struct TriggerBlock {
	State **states; // Array of states
	size_t stateCount;
	Transition **transitions; // Array of transitions
	size_t transitionCount;
};

struct State {
	char *name; // e.g., "default"
	Style *style; // Associated style
};

struct Transition {
	char *fromState; // e.g., "default"
	char *toState; // e.g., "expanded"
	Animate *animate; // Associated animation
	Style *style;
};

struct Style {
	Property **properties; // Array of style properties
	size_t propertyCount;
};

struct Animate {
	char *duration; // e.g., "300ms"
	char *easing; // e.g., "cubic-bezier(0.4, 0.0, 0.2, 1)"
};

struct Property {
	char *name; // e.g., "height"
	char *value; // e.g., "200px"
};

struct Program {
	TriggerList * triggerList;
};

struct TriggerList {
	Trigger **trigger;
	size_t triggerCount;
};

/**
 * Node recursive destructors.
 */
void releaseProgram(Program * program);
void releaseTrigger(Trigger * trigger);
void releaseTriggerBlock(TriggerBlock * triggerBlock);
void releaseTriggerList(TriggerList * triggerList);
void releaseState(State * state);
void releaseTransition(Transition * transition);
void releaseStyle(Style * style);
void releaseAnimate(Animate * animate);
void releaseProperty(Property * property);

#endif
