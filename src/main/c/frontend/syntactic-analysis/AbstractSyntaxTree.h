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
typedef enum TransitionRuleType TransitionRuleType;
typedef enum AliasType AliasType;
typedef enum TransitionBlockItemType TransitionBlockItemType;
typedef enum AnimateType AnimateType;

typedef struct Program Program;
typedef struct Trigger Trigger;
typedef struct State State;
typedef struct Transition Transition;
typedef struct TransitionBlock TransitionBlock;
typedef struct TransitionRule TransitionRule;
typedef struct Style Style;
typedef struct KeyframeStyle KeyframeStyle;
typedef struct KeyframeStyleList KeyframeStyleList;
typedef struct Animate Animate;
typedef struct Keyframes Keyframes;
typedef struct Property Property;
typedef struct Group Group;
typedef struct Stagger Stagger;
typedef struct Sequence Sequence;
typedef struct TriggerList TriggerList;
typedef struct StateList StateList;
typedef struct TransitionList TransitionList;
typedef struct PropertyList PropertyList;
typedef struct TriggerBlock TriggerBlock;
typedef struct TransitionRule TransitionRule;
typedef struct TransitionBlockItem TransitionBlockItem;
typedef struct TransitionBlockItemList TransitionBlockItemList;

enum Direction {
	FORWARD,
	BIDIRECTIONAL,
};

enum PropertyType {
    STRING,
    FLOAT,
    INT,
};

enum TransitionRuleType {
    FROM_TO,
    ALIAS,
};

enum AliasType {
    ENTER,
    LEAVE,
    INCREMENT,
    DECREMENT,
};

enum TransitionBlockItemType {
    ANIMATE_ITEM,
    STYLE_ITEM,
    GROUP_ITEM,
    SEQUENCE_ITEM,
    STAGGER_ITEM,
};

enum AnimateType {
    NONE,
    ANIMATE_WITH_STYLE,
    ANIMATE_WITH_KEYFRAMES,
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
    union {
        struct {
            char *fromState; // e.g., "default"
            char *toState; // e.g., "expanded"
            Direction direction; // e.g., "FORWARD"
        };
        AliasType alias; // e.g., ":enter" //TODO check if it is better to use char * or enum for these reserved (i think enum)
    };
    TransitionRuleType ruleType;
};

struct TransitionBlock { //TODO: this might need simplifying, it only calls the list, might this be the list instead? applies for everything that is a list
    TransitionBlockItemList * transitionBlockItemList; // List of transition block items
};

struct TransitionBlockItemList {
    TransitionBlockItem **items; // Array of items (Animate, Style, etc.)
    size_t itemCount;
};

typedef struct TransitionBlockItem { //TODO: if i am right this might need a rename
    TransitionBlockItemType type; // TODO: same as above
    void *item; // Pointer to either Animate or Style
} TransitionBlockItem;

struct Style {
	PropertyList *properties;
};

struct KeyframeStyle {
    PropertyList *properties; // Reuse the existing Style struct
    float offset;    // Offset for the keyframe (e.g., 0.0 to 1.0)
};

struct KeyframeStyleList {
    KeyframeStyle **keyframeStyles; // Array of keyframe styles
    size_t keyframeCount;
};

struct PropertyList {
	Property **properties; // Array of properties
	size_t propertyCount;
};

struct Keyframes {
    KeyframeStyleList *keyframeStyleList;
};

struct Animate {
    union {
        Style * style;
        Keyframes * keyframes;
    };
    AnimateType type; // Type of animation (none, with style, with keyframe)
	char *duration; // e.g., "300ms"
	char *easing; // e.g., "cubic-bezier(0.4, 0.0, 0.2, 1)"
};

struct Group { //TODO: this, along with sequence and stagger (which i hope exists by the time you read this) accept the same things... look into that IV0 and SantIAgo
    TransitionBlockItemList * transitionBlockItemList; // List of transition block items
};

struct Sequence {
    TransitionBlockItemList * transitionBlockItemList; // List of transition block items
};

struct Stagger {
    char *time; // e.g., "100ms"
    TransitionBlockItemList * transitionBlockItemList; // List of transition block items
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
void releaseTransitionBlockItemList(TransitionBlockItemList * transitionBlockItemList);
void releaseTransitionBlockItem(TransitionBlockItem * transitionBlockItem);
void releaseTransitionList(TransitionList * transitionList);
void releaseTransitionRule(TransitionRule * transitionRule);
void releaseStyle(Style * style);
void releaseKeyframeStyle(KeyframeStyle * keyframeStyle);
void releaseKeyframeStyleList(KeyframeStyleList * keyframeStyleList);
void releaseKeyframes(Keyframes * keyframes);
void releasePropertyList(PropertyList * propertyList);
void releaseAnimate(Animate * animate);
void releaseProperty(Property * property);
void releaseGroup(Group * group);
void releaseSequence(Sequence * sequence);
void releaseStagger(Stagger * stagger);

#endif
