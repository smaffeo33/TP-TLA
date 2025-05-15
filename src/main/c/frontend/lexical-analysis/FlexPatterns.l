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
COLOR       #[0-9a-fA-F]{6}
TIME        {NUMBER}{TIME_UNIT}
VALUE_UNIT  "px"|"em"|"%"|"rem"|"vh"|"vw"
VALUE       {NUMBER}{VALUE_UNIT}
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

"{"                                                 { return DelimiterAction(createLexicalContext(), OPEN_BRACE); }
"}"                                                 { return DelimiterAction(createLexicalContext(), CLOSE_BRACE); }

"\'"                                                {
                                                        BEGIN(APOSTROPHE_STRING);
                                                        return StringLiteralAction(createLexicalContext(), APOSTROPHE);
                                                    }

<APOSTROPHE_STRING>"\'"                             {
                                                        BEGIN(INITIAL);
                                                        return StringLiteralAction(createLexicalContext(), APOSTROPHE);
                                                    }

<APOSTROPHE_STRING>{TIME}                           { return TimeValueAction(createLexicalContext()); }
<APOSTROPHE_STRING>{EASING}                         { return EasingFunctionAction(createLexicalContext()); }
<APOSTROPHE_STRING>"=>"                             { return TransitionArrowAction(createLexicalContext(), FORWARD_TRANSITION); }
<APOSTROPHE_STRING>"<=>"                            { return TransitionArrowAction(createLexicalContext(), BIDIRECTIONAL_TRANSITION); }
<APOSTROPHE_STRING>{ID}                             { return NameAction(createLexicalContext()); }
<APOSTROPHE_STRING>{COLOR}                          { return ColorAction(createLexicalContext()); }

"\""                                                {
                                                        BEGIN(QUOTATION_MARK_STRING);
                                                        return StringLiteralAction(createLexicalContext(), QUOTATION_MARK);
                                                    }

<APOSTROPHE_STRING>"\""                             {
                                                        BEGIN(INITIAL);
                                                        return StringLiteralAction(createLexicalContext(), QUOTATION_MARK);
                                                    }

<QUOTATION_MARK_STRING>{TIME}                       { return TimeValueAction(createLexicalContext()); }
<QUOTATION_MARK_STRING>{EASING}                     { return EasingFunctionAction(createLexicalContext()); }
<QUOTATION_MARK_STRING>"=>"                         { return TransitionArrowAction(createLexicalContext(), FORWARD_TRANSITION); }
<QUOTATION_MARK_STRING>"<=>"                        { return TransitionArrowAction(createLexicalContext(), BIDIRECTIONAL_TRANSITION); }
<QUOTATION_MARK_STRING>{ID}                         { return NameAction(createLexicalContext()); }
<QUOTATION_MARK_STRING>{COLOR}                      { return ColorAction(createLexicalContext()); }

{ID}                                                { return StylePropertyAction(createLexicalContext()); }
{NUMBER}                                            { return ValueAction(createLexicalContext()); }

","                                                 { return CommaAction(createLexicalContext()); }
":"                                                 { return ColonAction(createLexicalContext()); }
"."                                                 { return DotAction(createLexicalContext()); }

{WHITESPACE}+                                       { IgnoredLexemeAction(createLexicalContext()); }
.                                                   { return UnknownLexemeAction(createLexicalContext()); }

%%

#include "FlexExport.h"