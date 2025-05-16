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



/** Terminals. */
%token <integer> INTEGER
%token <token> NUMBER
%token <token> VALUE
%token <token> TIME
%token <token> COLOR_VALUE
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
%token <token> EASING
%token <token> LEAVE_QUERY
%token <token> ENTER_QUERY
%token <token> VOID_STATE
%token <token> WILDCARD_STATE

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

triggerList: trigger                                                                { $$ = TriggerSemanticAction($1); }
    | triggerList COMMA trigger                                                     { $$ = TriggerListSemanticAction($1, $3); }
    ;

trigger: OPEN_PARENTHESIS APOSTROPHE NAME APOSTROPHE COMMA triggerBlock CLOSE_PARENTHESIS
                                                                                    { $$ = TriggerSemanticAction($3, $6); }
    ;

triggerBlock: OPEN_BRACKET stateList COMMA transitionList CLOSE_BRACKET             { $$ = TriggerBlockSemanticAction($2, $4); }
    ;

stateList: state                                                                    { $$ = StateSemanticAction($1); }
    | stateList COMMA state                                                         { $$ = StateListSemanticAction($1, $3); }
    ;

transitionList: transition                                                          { $$ = TransitionSemanticAction($1); }
    | transitionList COMMA transition                                               { $$ = TransitionListSemanticAction($1, $3); }
    ;

state: OPEN_PARENTHESIS APOSTROPHE NAME APOSTROPHE COMMA style CLOSE_PARENTHESIS    { $$ = StateDefinitionSemanticAction($3, $6); }
    ;

transition: OPEN_PARENTHESIS APOSTROPHE NAME FORWARD_TRANSITION NAME APOSTROPHE COMMA animate CLOSE_PARENTHESIS
                                                                                    { $$ = DirectionTransitionSemanticAction($3, $4, $5, $8); }
    | OPEN_PARENTHESIS APOSTROPHE NAME BIDIRECTIONAL_TRANSITION NAME APOSTROPHE COMMA animate CLOSE_PARENTHESIS
                                                                                    { $$ = DirectionTransitionSemanticAction($3, $4, $5, $8); }
    ;

style: OPEN_PARENTHESIS OPEN_BRACE propertyList CLOSE_BRACE CLOSE_PARENTHESIS      { $$ = StyleSemanticAction($3); }
    ;

propertyList: property                                                              { $$ = PropertySemanticAction($1); }
    | propertyList COMMA property                                                   { $$ = PropertyListSemanticAction($1, $3); }
    ;

property: NAME COLON APOSTROPHE VALUE APOSTROPHE                                    { $$ = ValuePropertySemanticAction($1, $4); }
    | NAME COLON APOSTROPHE COLOR_VALUE APOSTROPHE                                  { $$ = ValuePropertySemanticAction($1, $4); }
    | NAME COLON APOSTROPHE NUMBER APOSTROPHE                                       { $$ = ValuePropertySemanticAction($1, $4); }
    ;

animate: OPEN_PARENTHESIS APOSTROPHE TIME EASING APOSTROPHE CLOSE_PARENTHESIS      { $$ = AnimateSemanticAction($3, $4); }
    ;

%%
