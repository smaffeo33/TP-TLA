%{

#include "BisonActions.h"

%}

// You touch this, and you die.
%define api.value.union.name SemanticValue

%union {
	/** Terminals. */

	Token token;
	char *string;
	float number;

	/** Non-terminals. */

	Program * program;
	Trigger * trigger;
	Transition * transition;
	State * state;
	Style * style;
	KeyframeStyle * keyframeStyle;
	KeyframeStyleList * keyframeStyleList;
	Keyframes * keyframes;
	Animate * animate;
	AnimateInfo * animateInfo;
	Property * property;
	Group * group;
	Sequence * sequence;
	Query * query;
	Stagger * stagger;
	TriggerList * triggerList;
	TriggerBlock * triggerBlock;
	StateList * stateList;
	PropertyList * propertyList;
	TransitionList * transitionList;
	TransitionBlock * transitionBlock;
	StepItem * stepItem;
	StepItemList * stepItemList;
	TransitionRule * transitionRule;
	AliasType aliasType;
	Direction direction;
}

/**
 * Destructors. This functions are executed after the parsing ends, so if the
 * AST must be used in the following phases of the compiler you shouldn't used
 * this approach for the AST root node ("program" non-terminal, in this
 * grammar), or it will drop the entire tree even if the parse succeeds.
 *
 * @see https://www.gnu.org/software/bison/manual/html_node/Destructor-Decl.html
 */
%destructor { releaseTrigger($$); } <trigger>
%destructor { releaseTriggerList($$); } <triggerList>
%destructor { releaseTriggerBlock($$); } <triggerBlock>
%destructor { releaseState($$); } <state>
%destructor { releaseStateList($$); } <stateList>
%destructor { releaseTransition($$); } <transition>
%destructor { releaseTransitionList($$); } <transitionList>
%destructor { releaseStyle($$); } <style>
%destructor { releaseAnimate($$); } <animate>
%destructor { releaseProperty($$); } <property>
%destructor { releasePropertyList($$); } <propertyList>
%destructor { releaseTransitionBlock($$); } <transitionBlock>
%destructor { releaseTransitionRule($$); } <transitionRule>
%destructor { releaseStepItem($$); } <stepItem>
%destructor { releaseStepItemList($$); } <stepItemList>
%destructor { releaseKeyframeStyle($$); } <keyframeStyle>
%destructor { releaseKeyframeStyleList($$); } <keyframeStyleList>
%destructor { releaseKeyframes($$); } <keyframes>
%destructor { releaseAnimateInfo($$); } <animateInfo>
%destructor { releaseGroup($$); } <group>
%destructor { releaseSequence($$); } <sequence>
%destructor { releaseStagger($$); } <stagger>
%destructor { releaseQuery($$); } <query>



/** Terminals. */
%token <number> NUMBER
%token <string> VALUE
%token <string> TIME
%token <string> STRING_VALUE
%token <string> COLOR_VALUE
%token <token> CLOSE_PARENTHESIS
%token <token> OPEN_PARENTHESIS
%token <token> CLOSE_BRACE
%token <token> OPEN_BRACE
%token <token> CLOSE_BRACKET
%token <token> OPEN_BRACKET
%token <token> FORWARD_TRANSITION
%token <token> BIDIRECTIONAL_TRANSITION
%token <token> APOSTROPHE
%token <token> QUOTATION_MARK
%token <token> TRIGGER
%token <token> STATE
%token <token> STYLE
%token <token> TRANSITION
%token <token> ANIMATE
%token <token> SEQUENCE
%token <token> GROUP
%token <token> QUERY
%token <token> STAGGER
%token <token> KEYFRAMES
%token <token> USE_ANIMATION
%token <token> ANIMATION
%token <token> ANIMATE_CHILD
%token <token> OFFSET
%token <token> COMMA
%token <token> COLON
%token <string> VALUE_PROPERTY
%token <string> COLOR_PROPERTY
%token <string> KEYWORD_PROPERTY
%token <string> KEYWORD_TRANSFORM_PROPERTY
%token <string> TRANSFORM_PROPERTY
%token <string> NUMBER_PROPERTY
%token <string> NAME
%token <string> EASING
%token <token> LEAVE_QUERY
%token <token> ENTER_QUERY
%token <token> VOID_STATE
%token <token> WILDCARD_STATE
%token <token> ENTER_ALIAS
%token <token> LEAVE_ALIAS
%token <token> INCREMENT_ALIAS
%token <token> DECREMENT_ALIAS
%token <string> SELECTOR_ID
%token <string> SELECTOR_CLASS


%token <token> UNKNOWN

/** Non-terminals. */
%type <trigger> trigger
%type <state> state
%type <transition> transition
%type <style> style
%type <keyframeStyle> keyframeStyle
%type <keyframes> keyframes
%type <keyframeStyleList> keyframeStyleList
%type <animate> animate
%type <animateInfo> animateInfo
%type <property> property
%type <group> group
%type <sequence> sequence
%type <stagger> stagger
%type <triggerList> triggerList
%type <triggerBlock> triggerBlock
%type <stateList> stateList
%type <transitionList> transitionList
%type <propertyList> propertyList
%type <program> program
%type <transitionBlock> transitionBlock
%type <stepItem> stepItem
%type <stepItem> queryStepItem
%type <stepItemList> stepItemList
%type <stepItemList> queryStepItemList
%type <transitionRule> transitionRule
%type <aliasType> aliasType
%type <direction> transitionType
%type <query> query

/**
 * Precedence and associativity.
 *
 * @see https://www.gnu.org/software/bison/manual/html_node/Precedence.html
 */

%%

// IMPORTANT: To use λ in the following grammar, use the %empty symbol.
//VER
program: triggerList                                                                { $$ = TriggerListProgramSemanticAction(currentCompilerState(), $1); }
    ;

triggerList: trigger                                                                { $$ = TriggerTriggerListSemanticAction($1); }
    | triggerList COMMA trigger                                                     { $$ = TriggerListSemanticAction($1, $3); }
    ;

trigger: TRIGGER OPEN_PARENTHESIS APOSTROPHE NAME APOSTROPHE COMMA triggerBlock CLOSE_PARENTHESIS
                                                                                    { $$ = TriggerSemanticAction($4, $7); }
    ;

triggerBlock: OPEN_BRACKET stateList COMMA transitionList CLOSE_BRACKET             { $$ = TriggerBlockSemanticAction($2, $4); }
    | OPEN_BRACKET transitionList CLOSE_BRACKET                                     { $$ = StatelessTriggerBlockSemanticAction($2); }
    ;

stateList: state                                                                    { $$ = StateStateListSemanticAction($1); }
    | stateList COMMA state                                                         { $$ = StateListSemanticAction($1, $3); }
    ;

transitionList: transition                                                          { $$ = TransitionTransitionListSemanticAction($1); }
    | transitionList COMMA transition                                               { $$ = TransitionListSemanticAction($1, $3); }
    ;

state: STATE OPEN_PARENTHESIS APOSTROPHE NAME APOSTROPHE COMMA style CLOSE_PARENTHESIS
                                                                                    { $$ = StateDefinitionSemanticAction($4, $7); }
    ;

transition: TRANSITION OPEN_PARENTHESIS APOSTROPHE transitionRule APOSTROPHE COMMA  transitionBlock  CLOSE_PARENTHESIS
                                                                                    { $$ = TransitionSemanticAction($4, $7); }
    ;

transitionRule: NAME transitionType NAME                                            { $$ = TransitionRuleSemanticAction($1, $3, $2); }
    | aliasType                                                                     { $$ = AliasTypeSemanticAction($1); }
    ;

transitionType: FORWARD_TRANSITION                                                  { $$ = FORWARD; }
    | BIDIRECTIONAL_TRANSITION                                                      { $$ = BIDIRECTIONAL; }
    ;

aliasType: ENTER_ALIAS                                                              { $$ = AliasSemanticAction(ENTER); }
    | LEAVE_ALIAS                                                                   { $$ = AliasSemanticAction(LEAVE); }
    | INCREMENT_ALIAS                                                               { $$ = AliasSemanticAction(INCREMENT); }
    | DECREMENT_ALIAS                                                               { $$ = AliasSemanticAction(DECREMENT); }
    ;

transitionBlock: animate                                                            { $$ = AnimateTransitionBlockSemanticAction($1); }
    | OPEN_BRACKET stepItemList CLOSE_BRACKET                            { $$ = StepItemListTransitionBlockSemanticAction($2); }
    ;

stepItemList: stepItem                                                              { $$ = StepItemListSemanticAction($1); }
    | stepItemList COMMA stepItem                                                   { $$ = StepItemStepItemListSemanticAction($1, $3); }
    ;

stepItem: animate                                                                   { $$ = AnimateStepItemSemanticAction($1); }
    | style                                                                         { $$ = StyleStepItemSemanticAction($1); }
    | group                                                                         { $$ = GroupStepItemSemanticAction($1); }
    | sequence                                                                      { $$ = SequenceStepItemSemanticAction($1); }
    | query                                                                         { $$ = QueryStepItemSemanticAction($1); }
    | ANIMATE_CHILD OPEN_PARENTHESIS CLOSE_PARENTHESIS                              { $$ = AnimateChildStepItemSemanticAction(); }
    ;

queryStepItemList: queryStepItem                                                    { $$ = StepItemListSemanticAction($1); }
    | queryStepItemList COMMA queryStepItem                                         { $$ = StepItemStepItemListSemanticAction($1, $3); }
    ;

queryStepItem: stepItem                                                             { $$ = $1; }
    | stagger                                                                       { $$ = StaggerStepItemSemanticAction($1); }
    ;

query: QUERY OPEN_PARENTHESIS APOSTROPHE SELECTOR_ID APOSTROPHE COMMA OPEN_BRACKET queryStepItemList CLOSE_BRACKET CLOSE_PARENTHESIS
                                                                                    { $$ = QuerySemanticAction($4, $8, ID); }
    | QUERY OPEN_PARENTHESIS APOSTROPHE SELECTOR_CLASS APOSTROPHE COMMA OPEN_BRACKET queryStepItemList CLOSE_BRACKET CLOSE_PARENTHESIS
                                                                                    { $$ = QuerySemanticAction($4, $8, CLASS); }
    | QUERY OPEN_PARENTHESIS APOSTROPHE NAME APOSTROPHE COMMA OPEN_BRACKET queryStepItemList CLOSE_BRACKET CLOSE_PARENTHESIS
                                                                                    { $$ = QuerySemanticAction($4, $8, TAG); }
    | QUERY OPEN_PARENTHESIS APOSTROPHE aliasType APOSTROPHE COMMA OPEN_BRACKET queryStepItemList CLOSE_BRACKET CLOSE_PARENTHESIS
                                                                                    { $$ = QueryAliasSemanticAction($4, $8); }

stagger: STAGGER OPEN_PARENTHESIS APOSTROPHE TIME APOSTROPHE COMMA OPEN_BRACKET stepItemList CLOSE_BRACKET CLOSE_PARENTHESIS
                                                                                    { $$ = StaggerSemanticAction($4, $8); }
    ;

sequence: SEQUENCE OPEN_PARENTHESIS OPEN_BRACKET stepItemList CLOSE_BRACKET CLOSE_PARENTHESIS
                                                                                    { $$ = SequenceSemanticAction($4); }

group: GROUP OPEN_PARENTHESIS OPEN_BRACKET stepItemList CLOSE_BRACKET CLOSE_PARENTHESIS
                                                                                    { $$ = GroupSemanticAction($4); }
    ;

style: STYLE OPEN_PARENTHESIS OPEN_BRACE propertyList CLOSE_BRACE CLOSE_PARENTHESIS      { $$ = StyleSemanticAction($4); }
    ;

propertyList: property                                                              { $$ = PropertyPropertyListSemanticAction($1); }
    | propertyList COMMA property                                                   { $$ = PropertyListSemanticAction($1, $3); }
    ;

property: VALUE_PROPERTY COLON VALUE                                                { $$ = ValuePropertySemanticAction($1, $3); }
    | COLOR_PROPERTY COLON COLOR_VALUE                                              { $$ = ColorValuePropertySemanticAction($1, $3); }
    | KEYWORD_PROPERTY COLON APOSTROPHE STRING_VALUE APOSTROPHE                     { $$ = TextValuePropertySemanticAction($1, $4); }
    | KEYWORD_TRANSFORM_PROPERTY COLON APOSTROPHE TRANSFORM_PROPERTY APOSTROPHE     { $$ = TextValuePropertySemanticAction($1, $4); }
    | NUMBER_PROPERTY COLON NUMBER                                                  { $$ = FloatValuePropertySemanticAction($1, $3); }
    ;


animate: ANIMATE OPEN_PARENTHESIS APOSTROPHE animateInfo APOSTROPHE CLOSE_PARENTHESIS   { $$ = AnimateSemanticAction($4); }
    | ANIMATE OPEN_PARENTHESIS APOSTROPHE animateInfo APOSTROPHE COMMA style CLOSE_PARENTHESIS
                                                                                        { $$ = AnimateWithStyleSemanticAction($4, $7); }
    | ANIMATE OPEN_PARENTHESIS APOSTROPHE animateInfo APOSTROPHE COMMA keyframes CLOSE_PARENTHESIS
                                                                                        { $$ = AnimateWithKeyframesSemanticAction($4, $7); }
    ;

animateInfo: TIME TIME EASING                                                           { $$ = AnimateInfoSemanticAction($1, $2, $3, DURATION_DELAY_EASING); }
    | TIME EASING                                                                       { $$ = AnimateInfoSemanticAction($1, NULL, $2, DURATION_EASING); }
    | TIME TIME                                                                         { $$ = AnimateInfoSemanticAction($1, $2, NULL, DURATION_DELAY); }
    | TIME                                                                              { $$ = AnimateInfoSemanticAction($1, NULL, NULL, DURATION); }

keyframes: KEYFRAMES OPEN_PARENTHESIS OPEN_BRACKET keyframeStyleList CLOSE_BRACKET CLOSE_PARENTHESIS
                                                                                        { $$ = KeyframesSemanticAction($4); }
    ;

keyframeStyleList: keyframeStyle
                                                                                        { $$ = KeyframeStyleKeyframeStyleListOffsetSemanticAction($1); }
    | keyframeStyleList COMMA keyframeStyle
                                                                                        { $$ = KeyframeStyleListSemanticAction($1, $3); }
    ;

keyframeStyle: STYLE OPEN_PARENTHESIS OPEN_BRACE propertyList COMMA OFFSET COLON NUMBER CLOSE_BRACE CLOSE_PARENTHESIS
                                                                                        { $$ = keyframeStyleSemanticAction($4, $8); }
    ;

%%
