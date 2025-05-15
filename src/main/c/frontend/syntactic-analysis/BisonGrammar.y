%{

#include "BisonActions.h"

%}

// You touch this, and you die.
%define api.value.union.name SemanticValue

%union {
	/** Terminals. */

	int integer;
	Token token;

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
}

/**
 * Destructors. This functions are executed after the parsing ends, so if the
 * AST must be used in the following phases of the compiler you shouldn't used
 * this approach for the AST root node ("program" non-terminal, in this
 * grammar), or it will drop the entire tree even if the parse succeeds.
 *
 * @see https://www.gnu.org/software/bison/manual/html_node/Destructor-Decl.html
 */
%destructor { releaseConstant($$); } <constant>
%destructor { releaseExpression($$); } <expression>
%destructor { releaseFactor($$); } <factor>

/** Terminals. */
%token <integer> INTEGER
%token <token> VALUE
%token <token> TIME_VALUE
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

%token <token> UNKNOWN

/** Non-terminals. */
%type <trigger> trigger
%type <state> state
%type <transition> transition
%type <style> style
%type <animate> animate
%type <property> property
%type <value> value
%type <triggerList> triggerList
%type <triggerBlock> triggerBlock
%type <program> program

/**
 * Precedence and associativity.
 *
 * @see https://www.gnu.org/software/bison/manual/html_node/Precedence.html
 */

%%

// IMPORTANT: To use λ in the following grammar, use the %empty symbol.

program: triggerList												            { $$ = ExpressionProgramSemanticAction(currentCompilerState(), $1); }
	;

triggerList: trigger			                                                { $$ = TriggerListSemanticAction($1, $3); }
    | triggerList COMMA trigger		                                            { $$ = TriggerListSemanticAction($1, $2); }
    ;

trigger: OPEN_PARENTHESIS APOSTROPHE NAME APOSTROPHE COMMA triggerBlock
    ;

expression: expression[left] ADD expression[right]					            { $$ = ArithmeticExpressionSemanticAction($left, $right, ADDITION); }
	| expression[left] DIV expression[right]						            { $$ = ArithmeticExpressionSemanticAction($left, $right, DIVISION); }
	| expression[left] MUL expression[right]						            { $$ = ArithmeticExpressionSemanticAction($left, $right, MULTIPLICATION); }
	| expression[left] SUB expression[right]						            { $$ = ArithmeticExpressionSemanticAction($left, $right, SUBTRACTION); }
	| factor														            { $$ = FactorExpressionSemanticAction($1); }
	;

factor: OPEN_PARENTHESIS expression CLOSE_PARENTHESIS				            { $$ = ExpressionFactorSemanticAction($2); }
	| constant														            { $$ = ConstantFactorSemanticAction($1); }
	;

constant: INTEGER													            { $$ = IntegerConstantSemanticAction($1); }
	;

%%
