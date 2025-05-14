%{

#include "AngularTransitionActions.h"

static YY_BUFFER_STATE importedTransitionBufferState = NULL;

%}

/**
 * Enable start condition manipulation functions.
 */
%option stack

/**
 * Flex contexts (a.k.a. start conditions).
 */
%x STYLE_OBJECT
%x ANIMATION_PARAMS
%x STATE_DEFINITION
%x TRANSITION_DEFINITION
%x MULTILINE_COMMENT

/**
 * Reusable patterns for Angular transitions
 */
DIGIT       [0-9]
ALPHA       [a-zA-Z]
ALPHANUM    [a-zA-Z0-9]
ID          [a-zA-Z][a-zA-Z0-9_]*
STRING      ['"][^'"]*['"]
NUMBER      {DIGIT}+(\.{DIGIT}+)?
TIME_UNIT   "ms"|"s"
TIME        {NUMBER}{TIME_UNIT}
EASING      "ease"|"ease-in"|"ease-out"|"ease-in-out"|"linear"|"cubic-bezier"{WHITESPACE}*\({WHITESPACE}*{NUMBER}{WHITESPACE}*,{WHITESPACE}*{NUMBER}{WHITESPACE}*,{WHITESPACE}*{NUMBER}{WHITESPACE}*,{WHITESPACE}*{NUMBER}{WHITESPACE}*\)
WHITESPACE  [ \t\r\n]

%%

"/*"                            { BEGIN(MULTILINE_COMMENT); BeginMultilineCommentAction(createLexicalContext()); }
<MULTILINE_COMMENT>"*/"         { EndMultilineCommentAction(createLexicalContext()); BEGIN(INITIAL); }
<MULTILINE_COMMENT>[^*]+        { IgnoredLexemeAction(createLexicalContext()); }
<MULTILINE_COMMENT>.            { IgnoredLexemeAction(createLexicalContext()); }

"trigger"                       { return TriggerKeywordAction(createLexicalContext()); }
"state"                         { return StateKeywordAction(createLexicalContext()); }
"style"                         { return StyleKeywordAction(createLexicalContext()); }
"transition"                    { return TransitionKeywordAction(createLexicalContext()); }
"animate"                       { return AnimateKeywordAction(createLexicalContext()); }
"sequence"                      { return SequenceKeywordAction(createLexicalContext()); }
"group"                         { return GroupKeywordAction(createLexicalContext()); }
"query"                         { return QueryKeywordAction(createLexicalContext()); }
"stagger"                       { return StaggerKeywordAction(createLexicalContext()); }
"keyframes"                     { return KeyframesKeywordAction(createLexicalContext()); }
"useAnimation"                  { return UseAnimationKeywordAction(createLexicalContext()); }
"animation"                     { return AnimationKeywordAction(createLexicalContext()); }

"void"                          { return VoidStateAction(createLexicalContext()); }
"*"                             { return WildcardStateAction(createLexicalContext()); }
":enter"                        { return EnterQueryAction(createLexicalContext()); }
":leave"                        { return LeaveQueryAction(createLexicalContext()); }

"=>"                            { return TransitionArrowAction(createLexicalContext(), FORWARD_TRANSITION); }
"<=>"                           { return TransitionArrowAction(createLexicalContext(), BIDIRECTIONAL_TRANSITION); }

"("                             { return ParenthesisAction(createLexicalContext(), OPEN_PARENTHESIS); }
")"                             { return ParenthesisAction(createLexicalContext(), CLOSE_PARENTHESIS); }

"["                             { return BracketAction(createLexicalContext(), OPEN_BRACKET); }
"]"                             { return BracketAction(createLexicalContext(), CLOSE_BRACKET); }

"{"                             {
                                  BEGIN(STYLE_OBJECT);
                                  return BraceAction(createLexicalContext(), OPEN_BRACE);
                                }

<STYLE_OBJECT>"}"               {
                                  BEGIN(INITIAL);
                                  return BraceAction(createLexicalContext(), CLOSE_BRACE);
                                }

<STYLE_OBJECT>{ID}              { return StylePropertyAction(createLexicalContext()); }
<STYLE_OBJECT>":"               { return ColonAction(createLexicalContext()); }
<STYLE_OBJECT>","               { return CommaAction(createLexicalContext()); }
<STYLE_OBJECT>{STRING}          { return StyleValueAction(createLexicalContext()); }
<STYLE_OBJECT>{NUMBER}          { return StyleNumberAction(createLexicalContext()); }
<STYLE_OBJECT>"px"|"em"|"%"|"rem"|"vh"|"vw" { return StyleUnitAction(createLexicalContext()); }

{TIME}                          { return TimeValueAction(createLexicalContext()); }
{EASING}                        { return EasingFunctionAction(createLexicalContext()); }

{STRING}                        { return StringLiteralAction(createLexicalContext()); }
{ID}                            { return IdentifierAction(createLexicalContext()); }
{NUMBER}                        { return NumberAction(createLexicalContext()); }

","                             { return CommaAction(createLexicalContext()); }
":"                             { return ColonAction(createLexicalContext()); }
"."                             { return DotAction(createLexicalContext()); }

{WHITESPACE}+                   { IgnoredLexemeAction(createLexicalContext()); }
.                               { return UnknownLexemeAction(createLexicalContext()); }

%%

#include "FlexExport.h"