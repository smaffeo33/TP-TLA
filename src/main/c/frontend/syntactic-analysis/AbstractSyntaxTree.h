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

typedef enum Direction Direction;
typedef enum PropertyType PropertyType;

typedef struct Program Program;
typedef struct Trigger Trigger;
typedef struct State State;
typedef struct Transition Transition;
typedef struct TransitionBlock TransitionBlock;
typedef struct TransitionRule TransitionRule;
typedef struct Style Style;
typedef struct Animate Animate;
typedef struct Property Property;
typedef struct TriggerList TriggerList;
typedef struct StateList StateList;
typedef struct TransitionList TransitionList;
typedef struct PropertyList PropertyList;
typedef struct TriggerBlock TriggerBlock;
typedef struct TransitionRule TransitionRule;

enum Direction {
	FORWARD,
	BIDIRECTIONAL,
};

enum PropertyType {
    STRING,
    FLOAT,
    INT,
};

struct Trigger {
	char *name; // e.g., "cardAnimation"
	TriggerBlock *block;
};

struct TriggerList {
	Trigger **trigger;
	size_t triggerCount;
};

struct TriggerBlock {
	StateList *stateList;
	TransitionList *transitionList;
};

struct TransitionList {
	Transition **transitions; // Array of transitions
	size_t transitionCount;
};

struct StateList {
	State **states; // Array of states
	size_t stateCount;
};

struct State {
	char *name; // e.g., "default"
	Style *style; // Associated style
};

struct Transition {
	TransitionRule * transitionRule;
	TransitionBlock * transitionBlock; // Associated block
};

struct TransitionRule {
    char *fromState; // e.g., "default"
    char *toState; // e.g., "expanded"
    Direction direction; // e.g., "FORWARD"
};

struct TransitionBlock {
    Animate *animate; // Associated animation
    Style *style;
};

struct Style {
	PropertyList *properties;
};

struct PropertyList {
	Property **properties; // Array of properties
	size_t propertyCount;
};

struct Animate {
	char *duration; // e.g., "300ms"
	char *easing; // e.g., "cubic-bezier(0.4, 0.0, 0.2, 1)"
};

struct Property {
	char *name; // e.g., "height"
	union {
        char *value; // e.g., "200px"
        float floatValue; // e.g., 200.0
        int intValue; // e.g., 1
    };
    PropertyType type; // Type of the property (string or float)
};

struct Program {
	TriggerList * triggerList;
};

/**
 * Node recursive destructors.
 */
void releaseProgram(Program * program);
void releaseTrigger(Trigger * trigger);
void releaseTriggerBlock(TriggerBlock * triggerBlock);
void releaseTriggerList(TriggerList * triggerList);
void releaseState(State * state);
void releaseStateList(StateList * stateList);
void releaseTransition(Transition * transition);
void releaseTransitionBlock(TransitionBlock * transitionBlock);
void releaseTransitionList(TransitionList * transitionList);
void releaseTransitionRule(TransitionRule * transitionRule);
void releaseStyle(Style * style);
void releasePropertyList(PropertyList * propertyList);
void releaseAnimate(Animate * animate);
void releaseProperty(Property * property);

#endif
