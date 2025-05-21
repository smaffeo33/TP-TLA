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
typedef enum StepItemType StepItemType;
typedef enum AnimateType AnimateType;
typedef enum AnimateInfoType AnimateInfoType;
typedef enum SelectorType SelectorType;

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
typedef struct AnimateInfo AnimateInfo;
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
typedef struct StepItem StepItem;
typedef struct StepItemList StepItemList;
typedef struct Query Query;

enum Direction {
	FORWARD,
	BIDIRECTIONAL,
};

enum PropertyType {
    STRING,
    FLOAT,
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

enum StepItemType {
    ANIMATE_ITEM,
    STYLE_ITEM,
    GROUP_ITEM,
    SEQUENCE_ITEM,
    QUERY_ITEM,
    STAGGER_ITEM,
    ANIMATE_CHILD_ITEM,
};

enum AnimateType {
    NONE,
    ANIMATE_WITH_STYLE,
    ANIMATE_WITH_KEYFRAMES,
};

enum AnimateInfoType {
    DURATION_DELAY,
    DURATION_EASING,
    DURATION_DELAY_EASING,
    DURATION,
};

enum SelectorType {
    CLASS,
    ID,
    TAG,
    ALIAS_SELECTOR,
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
        AliasType alias; // e.g., ":enter"
    };
    TransitionRuleType ruleType;
};

struct TransitionBlock {
    StepItemList * stepItemList; // List of transition block items
};

struct StepItemList {
    StepItem **items; // Array of items (Animate, Style, etc.)
    size_t itemCount;
};

typedef struct StepItem {
    StepItemType type;
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
	AnimateInfo * animateInfo; // Animation information (duration, easing)
};

struct AnimateInfo {
    char *duration; // e.g., "300ms"
    char *easing; // e.g., "cubic-bezier(0.4, 0.0, 0.2, 1)"
    char *delay; // e.g., "100ms"
    AnimateInfoType type; // Type of animation information (delay, easing, delay + easing)

};

struct Group {
    StepItemList *stepItemList; // List of transition block items
};

struct Sequence {
    StepItemList *stepItemList; // List of transition block items
};

struct Query {
    union {
        char *selector;
        AliasType alias;
    };
    SelectorType selectorType;
    StepItemList *stepItemList; // List of transition block items
};

struct Stagger {
    char *time; // e.g., "100ms"
    StepItemList *stepItemList; // List of transition block items
};

struct Property {
	char *name; // e.g., "height"
	union {
        char *value; // e.g., "200px"
        float floatValue; // e.g., 200.0
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
void releaseStepItemList(StepItemList * stepItemList);
void releaseStepItem(StepItem * stepItem);
void releaseTransitionList(TransitionList * transitionList);
void releaseTransitionRule(TransitionRule * transitionRule);
void releaseStyle(Style * style);
void releaseKeyframeStyle(KeyframeStyle * keyframeStyle);
void releaseKeyframeStyleList(KeyframeStyleList * keyframeStyleList);
void releaseKeyframes(Keyframes * keyframes);
void releasePropertyList(PropertyList * propertyList);
void releaseAnimate(Animate * animate);
void releaseAnimateInfo(AnimateInfo * animateInfo);
void releaseProperty(Property * property);
void releaseGroup(Group * group);
void releaseSequence(Sequence * sequence);
void releaseStagger(Stagger * stagger);
void releaseQuery(Query * query);

#endif
