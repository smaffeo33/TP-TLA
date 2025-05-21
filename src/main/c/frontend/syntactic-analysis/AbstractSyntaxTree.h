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
	char *name; 
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
	Transition **transitions; 
	size_t transitionCount;
};

struct StateList {
	State **states; 
	size_t stateCount;
};

struct State {
	char *name; 
	Style *style; 
};

struct Transition {
	TransitionRule * transitionRule;
	TransitionBlock * transitionBlock; 
};

struct TransitionRule {
    union {
        struct {
            char *fromState; 
            char *toState; 
            Direction direction; 
        };
        AliasType alias; 
    };
    TransitionRuleType ruleType;
};

struct TransitionBlock {
    StepItemList * stepItemList; 
};

struct StepItemList {
    StepItem **items; 
    size_t itemCount;
};

typedef struct StepItem {
    StepItemType type;
    void *item; 
} TransitionBlockItem;

struct Style {
	PropertyList *properties;
};

struct KeyframeStyle {
    PropertyList *properties; 
    float offset;    
};

struct KeyframeStyleList {
    KeyframeStyle **keyframeStyles; 
    size_t keyframeCount;
};

struct PropertyList {
	Property **properties; 
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
    AnimateType type; 
	AnimateInfo * animateInfo; 
};

struct AnimateInfo {
    char *duration; 
    char *easing; 
    char *delay; 
    AnimateInfoType type; 

};

struct Group {
    StepItemList *stepItemList; 
};

struct Sequence {
    StepItemList *stepItemList; 
};

struct Query {
    union {
        char *selector;
        AliasType alias;
    };
    SelectorType selectorType;
    StepItemList *stepItemList; 
};

struct Stagger {
    char *time; 
    StepItemList *stepItemList; 
};

struct Property {
	char *name; 
	union {
        char *value; 
        float floatValue; 
    };
    PropertyType type; 
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
