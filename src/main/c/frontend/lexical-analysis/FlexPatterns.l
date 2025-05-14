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

"/*"                                                { BEGIN(MULTILINE_COMMENT); BeginMultilineCommentAction(createLexicalContext()); }
<MULTILINE_COMMENT>"*/"                             { EndMultilineCommentAction(createLexicalContext()); BEGIN(INITIAL); }
<MULTILINE_COMMENT>[^*]+                            { IgnoredLexemeAction(createLexicalContext()); }
<MULTILINE_COMMENT>.                                { IgnoredLexemeAction(createLexicalContext()); }

"trigger"                                           { return KeywordLexemeAction(createLexicalContext(), TRIGGER); }
"state"                                             { return KeywordLexemeAction(createLexicalContext(), STATE); }
"style"                                             { return KeywordLexemeAction(createLexicalContext(), STYLE); }
"transition"                                        { return KeywordLexemeAction(createLexicalContext(), TRANSITION); }
"animate"                                           { return KeywordLexemeAction(createLexicalContext(), ANIMATE); }
"sequence"                                          { return KeywordLexemeAction(createLexicalContext(), SEQUENCE); }
"group"                                             { return KeywordLexemeAction(createLexicalContext(), GROUP); }
"query"                                             { return KeywordLexemeAction(createLexicalContext(), QUERY); }
"stagger"                                           { return KeywordLexemeAction(createLexicalContext(), STAGGER); }
"keyframes"                                         { return KeywordLexemeAction(createLexicalContext(), KEYFRAMES); }
"useAnimation"                                      { return KeywordLexemeAction(createLexicalContext(), USE_ANIMATION); }
"animation"                                         { return KeywordLexemeAction(createLexicalContext(), ANIMATION); }

"void"                                              { return VoidStateAction(createLexicalContext()); }
"*"                                                 { return WildcardStateAction(createLexicalContext()); }
":enter"                                            { return EnterQueryAction(createLexicalContext()); }
":leave"                                            { return LeaveQueryAction(createLexicalContext()); }

"=>"                                                { return TransitionArrowAction(createLexicalContext(), FORWARD_TRANSITION); }
"<=>"                                               { return TransitionArrowAction(createLexicalContext(), BIDIRECTIONAL_TRANSITION); }

"("                                                 { return DelimiterAction(createLexicalContext(), OPEN_PARENTHESIS); }
")"                                                 { return DelimiterAction(createLexicalContext(), CLOSE_PARENTHESIS); }

"["                                                 { return DelimiterAction(createLexicalContext(), OPEN_BRACKET); }
"]"                                                 { return DelimiterAction(createLexicalContext(), CLOSE_BRACKET); }

"{"                                                 {
                                                      BEGIN(STYLE_OBJECT);
                                                      return DelimiterAction(createLexicalContext(), OPEN_BRACE);
                                                    }

<STYLE_OBJECT>"}"                                   {
                                                      BEGIN(INITIAL);
                                                      return DelimiterAction(createLexicalContext(), CLOSE_BRACE);
                                                    }

<STYLE_OBJECT>{ID}                                  { return StylePropertyAction(createLexicalContext()); }
<STYLE_OBJECT>":"                                   { return ColonAction(createLexicalContext()); }
<STYLE_OBJECT>","                                   { return CommaAction(createLexicalContext()); }
<STYLE_OBJECT>{STRING}                              { return StyleValueAction(createLexicalContext()); }
<STYLE_OBJECT>{NUMBER}                              { return StyleNumberAction(createLexicalContext()); }
<STYLE_OBJECT>"px"|"em"|"%"|"rem"|"vh"|"vw"         { return StyleUnitAction(createLexicalContext()); }

{TIME}                                              { return TimeValueAction(createLexicalContext()); }
{EASING}                                            { return EasingFunctionAction(createLexicalContext()); }

{STRING}                                            { return StringLiteralAction(createLexicalContext()); }
{ID}                                                { return IdentifierAction(createLexicalContext()); }
{NUMBER}                                            { return NumberAction(createLexicalContext()); }

","                                                 { return CommaAction(createLexicalContext()); }
":"                                                 { return ColonAction(createLexicalContext()); }
"."                                                 { return DotAction(createLexicalContext()); }

{WHITESPACE}+                                       { IgnoredLexemeAction(createLexicalContext()); }
.                                                   { return UnknownLexemeAction(createLexicalContext()); }

%%

#include "FlexExport.h"