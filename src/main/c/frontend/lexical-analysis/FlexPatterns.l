%{

#include "FlexActions.h"
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
%x APOSTROPHE_STRING
%x QUOTATION_MARK_STRING
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
INTEGER     {DIGIT}+
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

"void"                                              { return StateAction(createLexicalContext(), VOID_STATE); }
"*"                                                 { return StateAction(createLexicalContext(), WILDCARD_STATE); }
":enter"                                            { return StateAction(createLexicalContext(), ENTER_QUERY); }
":leave"                                            { return StateAction(createLexicalContext(), LEAVE_QUERY); }

"=>"                                                { return PunctuationAction(createLexicalContext(), FORWARD_TRANSITION); }
"<=>"                                               { return PunctuationAction(createLexicalContext(), BIDIRECTIONAL_TRANSITION); }

"("                                                 { return PunctuationAction(createLexicalContext(), OPEN_PARENTHESIS); }
")"                                                 { return PunctuationAction(createLexicalContext(), CLOSE_PARENTHESIS); }

"["                                                 { return PunctuationAction(createLexicalContext(), OPEN_BRACKET); }
"]"                                                 { return PunctuationAction(createLexicalContext(), CLOSE_BRACKET); }

"{"                                                 { return PunctuationAction(createLexicalContext(), OPEN_BRACE); }
"}"                                                 { return PunctuationAction(createLexicalContext(), CLOSE_BRACE); }

","                                                 { return PunctuationAction(createLexicalContext(), COMMA); }
":"                                                 { return PunctuationAction(createLexicalContext(), COLON); }
"."                                                 { return PunctuationAction(createLexicalContext(), DOT); }

"\'"                                                {
                                                        BEGIN(APOSTROPHE_STRING);
                                                        return StringLiteralAction(createLexicalContext(), APOSTROPHE);
                                                    }

<APOSTROPHE_STRING>"\'"                             {
                                                        BEGIN(INITIAL);
                                                        return StringLiteralAction(createLexicalContext(), APOSTROPHE);
                                                    }

<APOSTROPHE_STRING>{TIME}                           { return AnimationAction(createLexicalContext(), TIME_VALUE); }
<APOSTROPHE_STRING>{EASING}                         { return AnimationAction(createLexicalContext(), EASING_FUNCTION); }
<APOSTROPHE_STRING>"=>"                             { return AnimationAction(createLexicalContext(), FORWARD_TRANSITION); }
<APOSTROPHE_STRING>"<=>"                            { return AnimationAction(createLexicalContext(), BIDIRECTIONAL_TRANSITION); }
<APOSTROPHE_STRING>{ID}                             { return AnimationAction(createLexicalContext(), NAME); }
<APOSTROPHE_STRING>{COLOR}                          { return AnimationAction(createLexicalContext(), COLOR_VALUE); }

"\""                                                {
                                                        BEGIN(QUOTATION_MARK_STRING);
                                                        return StringLiteralAction(createLexicalContext(), QUOTATION_MARK);
                                                    }

<QUOTATION_MARK_STRING>"\""                         {
                                                        BEGIN(INITIAL);
                                                        return StringLiteralAction(createLexicalContext(), QUOTATION_MARK);
                                                    }

<QUOTATION_MARK_STRING>{TIME}                       { return AnimationAction(createLexicalContext(), TIME_VALUE); }
<QUOTATION_MARK_STRING>{EASING}                     { return AnimationAction(createLexicalContext(), EASING_FUNCTION); }
<QUOTATION_MARK_STRING>"=>"                         { return AnimationAction(createLexicalContext(), FORWARD_TRANSITION); }
<QUOTATION_MARK_STRING>"<=>"                        { return AnimationAction(createLexicalContext(), BIDIRECTIONAL_TRANSITION); }
<QUOTATION_MARK_STRING>{ID}                         { return AnimationAction(createLexicalContext(), NAME); }
<QUOTATION_MARK_STRING>{COLOR}                      { return AnimationAction(createLexicalContext(), COLOR_VALUE); }

{ID}                                                { return StylePropertyAction(createLexicalContext()); }
{INTEGER}                                           { return IntegerAction(createLexicalContext()); }

{WHITESPACE}+                                       { IgnoredLexemeAction(createLexicalContext()); }
.                                                   { return UnknownLexemeAction(createLexicalContext()); }

%%

#include "FlexExport.h"