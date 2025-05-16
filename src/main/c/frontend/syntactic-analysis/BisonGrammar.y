%{

#include "BisonActions.h"

%}

// You touch this, and you die.
%define api.value.union.name SemanticValue

%union {
	/** Terminals. */

	int integer;
	Token token;
	char *string;
	float number;

	/** Non-terminals. */

	Program * program;
	Trigger * trigger;
	Transition * transition;
	State * state;
	Style * style;
	Animate * animate;
	Property * property;
	TriggerList * triggerList;
	TriggerBlock * triggerBlock;
	StateList * stateList;
	PropertyList * propertyList;
	TransitionList * transitionList;
	TransitionBlock * transitionBlock;
	TransitionRule * transitionRule;
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



/** Terminals. */
%token <integer> INTEGER
%token <number> NUMBER
%token <string> VALUE
%token <string> TIME
%token <string> COLOR
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
%token <token> COMMA
%token <token> COLON
%token <string> NAME
%token <string> EASING
%token <token> LEAVE_QUERY
%token <token> ENTER_QUERY
%token <token> VOID_STATE
%token <token> WILDCARD_STATE
%token <string> RESERVED_STATE

%token <token> UNKNOWN

/** Non-terminals. */
%type <trigger> trigger
%type <state> state
%type <transition> transition
%type <style> style
%type <animate> animate
%type <property> property
%type <triggerList> triggerList
%type <triggerBlock> triggerBlock
%type <stateList> stateList
%type <transitionList> transitionList
%type <propertyList> propertyList
%type <program> program
%type <transitionBlock> transitionBlock
%type <transitionRule> transitionRule

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

state: STATE OPEN_PARENTHESIS APOSTROPHE NAME APOSTROPHE COMMA style CLOSE_PARENTHESIS    { $$ = StateDefinitionSemanticAction($4, $7); }
    ;

transition: TRANSITION OPEN_PARENTHESIS APOSTROPHE transitionRule APOSTROPHE COMMA  transitionBlock  CLOSE_PARENTHESIS
                                                                                    { $$ = TransitionSemanticAction($4, $7); }
    ;

transitionRule: NAME FORWARD_TRANSITION NAME                                        { $$ = ForwardTransitionRuleSemanticAction($1, $3); }
    | NAME BIDIRECTIONAL_TRANSITION NAME                                            { $$ = BidirectionalTransitionRuleSemanticAction($1, $3); }
    ;

transitionBlock: animate                                                            { $$ = AnimateTransitionBlockSemanticAction($1); }
    | OPEN_BRACKET animate CLOSE_BRACKET                                            { $$ = AnimateTransitionBlockSemanticAction($2); }
    | OPEN_BRACKET style COMMA animate CLOSE_BRACKET                                { $$ = StyleAnimateTransitionBlockSemanticAction($2, $4); }
    ;

style: STYLE OPEN_PARENTHESIS OPEN_BRACE propertyList CLOSE_BRACE CLOSE_PARENTHESIS      { $$ = StyleSemanticAction($4); }
    ;

propertyList: property                                                              { $$ = PropertyPropertyListSemanticAction($1); }
    | propertyList COMMA property                                                   { $$ = PropertyListSemanticAction($1, $3); }
    ;

property: NAME COLON VALUE                                                          { $$ = ValuePropertySemanticAction($1, $3); }
    | NAME COLON COLOR                                                              { $$ = ValuePropertySemanticAction($1, $3); }
    | NAME COLON NUMBER                                                             { $$ = FloatValuePropertySemanticAction($1, $3); }
    | NAME COLON INTEGER                                                            { $$ = IntegerValuePropertySemanticAction($1, $3); }
    ;



animate: ANIMATE OPEN_PARENTHESIS APOSTROPHE TIME EASING APOSTROPHE CLOSE_PARENTHESIS      { $$ = AnimateSemanticAction($4, $5); }
    ;

%%
