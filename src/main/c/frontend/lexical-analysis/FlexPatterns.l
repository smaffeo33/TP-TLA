%{

#include "FlexActions.h"

%}

/**
 * Enable start condition manipulation functions.
 */
%option stack

/**
 * Flex contexts (a.k.a. start conditions).
 */

%x MULTILINE_COMMENT
%x STRING_CONTEXT

/**
 * Reusable patterns for Angular transitions
 */
WHITESPACE          [ \t\r\n]
DIGIT               [0-9]
ALPHA               [a-zA-Z]
ALPHANUM            [a-zA-Z0-9]
ID                  ([a-zA-Z][a-zA-Z0-9_]*|\*)
STRING_VALUE        [a-zA-Z0-9\(\)]+
NUMBER              {DIGIT}+(\.{DIGIT}+)?
TIME_UNIT           "ms"|"s"
COLOR               (#[0-9a-fA-F]{6}|#[0-9a-fA-F]{3}|rgba\([ \t\r\n]*{NUMBER}[ \t\r\n]*,[ \t\r\n]*{NUMBER}[ \t\r\n]*,[ \t\r\n]*{NUMBER}[ \t\r\n]*,[ \t\r\n]*{NUMBER}[ \t\r\n]*\)|rgb\([ \t\r\n]*{NUMBER}[ \t\r\n]*,[ \t\r\n]*{NUMBER}[ \t\r\n]*,[ \t\r\n]*{NUMBER}[ \t\r\n]*\))
VALUE_UNIT          "px"|"em"|"%"|"rem"|"vh"|"vw"
EASING              "ease"|"ease-in"|"ease-out"|"ease-in-out"|"linear"|"cubic-bezier"{WHITESPACE}*\({WHITESPACE}*{NUMBER}{WHITESPACE}*,{WHITESPACE}*{NUMBER}{WHITESPACE}*,{WHITESPACE}*{NUMBER}{WHITESPACE}*,{WHITESPACE}*{NUMBER}{WHITESPACE}*\)
SELECTOR_ID         #{ALPHANUM}+
SELECTOR_CLASS     \.{ALPHANUM}+
SELECTOR_TAG        {ALPHA}+

%%

"/*"                                                { BEGIN(MULTILINE_COMMENT); BeginMultilineCommentLexemeAction(createLexicalAnalyzerContext()); }
<MULTILINE_COMMENT>"*/"                             { EndMultilineCommentLexemeAction(createLexicalAnalyzerContext()); BEGIN(INITIAL); }
<MULTILINE_COMMENT>[^*]+                            { IgnoredLexemeAction(createLexicalAnalyzerContext()); }
<MULTILINE_COMMENT>.                                { IgnoredLexemeAction(createLexicalAnalyzerContext()); }

"\'"                                                { BEGIN(STRING_CONTEXT); return StringLiteralAction(createLexicalAnalyzerContext(), APOSTROPHE); }
<STRING_CONTEXT>"\'"                                {  BEGIN(INITIAL); return StringLiteralAction(createLexicalAnalyzerContext(), APOSTROPHE); }
<STRING_CONTEXT>{WHITESPACE}+                       { IgnoredLexemeAction(createLexicalAnalyzerContext()); }
<STRING_CONTEXT>{NUMBER}{TIME_UNIT}                 { return TimeValueAction(createLexicalAnalyzerContext()); }
<STRING_CONTEXT>{EASING}                            { return AnimationAction(createLexicalAnalyzerContext()); }
<STRING_CONTEXT>":enter"                            { return AliasStateAction(createLexicalAnalyzerContext(), ENTER_ALIAS); }
<STRING_CONTEXT>":leave"                            { return AliasStateAction(createLexicalAnalyzerContext(), LEAVE_ALIAS); }
<STRING_CONTEXT>":increment"                        { return AliasStateAction(createLexicalAnalyzerContext(), INCREMENT_ALIAS); }
<STRING_CONTEXT>":decrement"                        { return AliasStateAction(createLexicalAnalyzerContext(), DECREMENT_ALIAS); }
<STRING_CONTEXT>{SELECTOR_ID}                       { return SelectorAction(createLexicalAnalyzerContext(), SELECTOR_ID); }
<STRING_CONTEXT>{SELECTOR_CLASS}                    { return SelectorAction(createLexicalAnalyzerContext(), SELECTOR_CLASS); }
<STRING_CONTEXT>{ID}                                { return NameAction(createLexicalAnalyzerContext()); }
<STRING_CONTEXT>"=>"                                { return PunctuationAction(createLexicalAnalyzerContext(), FORWARD_TRANSITION); }
<STRING_CONTEXT>"<=>"                               { return PunctuationAction(createLexicalAnalyzerContext(), BIDIRECTIONAL_TRANSITION); }
<STRING_CONTEXT>[^' \t\r\n]+                        { return StringValueAction(createLexicalAnalyzerContext()); }
<STRING_CONTEXT>.                                   { return UnknownLexemeAction(createLexicalAnalyzerContext()); }

"trigger"                                           { return KeywordLexemeAction(createLexicalAnalyzerContext(), TRIGGER); }
"state"                                             { return KeywordLexemeAction(createLexicalAnalyzerContext(), STATE); }
"style"                                             { return KeywordLexemeAction(createLexicalAnalyzerContext(), STYLE); }
"transition"                                        { return KeywordLexemeAction(createLexicalAnalyzerContext(), TRANSITION); }
"animate"                                           { return KeywordLexemeAction(createLexicalAnalyzerContext(), ANIMATE); }
"sequence"                                          { return KeywordLexemeAction(createLexicalAnalyzerContext(), SEQUENCE); }
"group"                                             { return KeywordLexemeAction(createLexicalAnalyzerContext(), GROUP); }
"query"                                             { return KeywordLexemeAction(createLexicalAnalyzerContext(), QUERY); }
"stagger"                                           { return KeywordLexemeAction(createLexicalAnalyzerContext(), STAGGER); }
"keyframes"                                         { return KeywordLexemeAction(createLexicalAnalyzerContext(), KEYFRAMES); }
"useAnimation"                                      { return KeywordLexemeAction(createLexicalAnalyzerContext(), USE_ANIMATION); }
"animation"                                         { return KeywordLexemeAction(createLexicalAnalyzerContext(), ANIMATION); }

"offset"                                            { return KeywordLexemeAction(createLexicalAnalyzerContext(), OFFSET); }



"("                                                 { return PunctuationAction(createLexicalAnalyzerContext(), OPEN_PARENTHESIS); }
")"                                                 { return PunctuationAction(createLexicalAnalyzerContext(), CLOSE_PARENTHESIS); }

"["                                                 { return PunctuationAction(createLexicalAnalyzerContext(), OPEN_BRACKET); }
"]"                                                 { return PunctuationAction(createLexicalAnalyzerContext(), CLOSE_BRACKET); }

"{"                                                 { return PunctuationAction(createLexicalAnalyzerContext(), OPEN_BRACE); }
"}"                                                 { return PunctuationAction(createLexicalAnalyzerContext(), CLOSE_BRACE); }

","                                                 { return PunctuationAction(createLexicalAnalyzerContext(), COMMA); }
":"                                                 { return PunctuationAction(createLexicalAnalyzerContext(), COLON); }

{NUMBER}{VALUE_UNIT}                                { return ValueAction(createLexicalAnalyzerContext()); }
{COLOR}                                             { return ColorValueAction(createLexicalAnalyzerContext()); }

{ID}                                                { return NameAction(createLexicalAnalyzerContext()); }

{NUMBER}                                            { return FloatAction(createLexicalAnalyzerContext()); }

{WHITESPACE}+                                       { IgnoredLexemeAction(createLexicalAnalyzerContext()); }

.                                                   { return UnknownLexemeAction(createLexicalAnalyzerContext()); }

%%

#include "FlexExport.h"