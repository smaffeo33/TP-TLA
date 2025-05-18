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

/**
 * Reusable patterns for Angular transitions
 */
WHITESPACE          [ \t\r\n]
DIGIT               [0-9]
ALPHA               [a-zA-Z]
ALPHANUM            [a-zA-Z0-9]
ID                  ([a-zA-Z][a-zA-Z0-9_]*|\*)
STRING              ['"][^'"]*['"]
NUMBER              {DIGIT}+(\.{DIGIT}+)?
INTEGER             {DIGIT}+
TIME_UNIT           "ms"|"s"
COLOR               (#[0-9a-fA-F]{6}|#[0-9a-fA-F]{3}|rgba\([ \t\r\n]*{NUMBER}[ \t\r\n]*,[ \t\r\n]*{NUMBER}[ \t\r\n]*,[ \t\r\n]*{NUMBER}[ \t\r\n]*,[ \t\r\n]*{NUMBER}[ \t\r\n]*\)|rgb\([ \t\r\n]*{NUMBER}[ \t\r\n]*,[ \t\r\n]*{NUMBER}[ \t\r\n]*,[ \t\r\n]*{NUMBER}[ \t\r\n]*\))
VALUE_UNIT          "px"|"em"|"%"|"rem"|"vh"|"vw"
EASING              "ease"|"ease-in"|"ease-out"|"ease-in-out"|"linear"|"cubic-bezier"{WHITESPACE}*\({WHITESPACE}*{NUMBER}{WHITESPACE}*,{WHITESPACE}*{NUMBER}{WHITESPACE}*,{WHITESPACE}*{NUMBER}{WHITESPACE}*,{WHITESPACE}*{NUMBER}{WHITESPACE}*\)


%%

"/*"                                                { BEGIN(MULTILINE_COMMENT); BeginMultilineCommentLexemeAction(createLexicalAnalyzerContext()); }
<MULTILINE_COMMENT>"*/"                             { EndMultilineCommentLexemeAction(createLexicalAnalyzerContext()); BEGIN(INITIAL); }
<MULTILINE_COMMENT>[^*]+                            { IgnoredLexemeAction(createLexicalAnalyzerContext()); }
<MULTILINE_COMMENT>.                                { IgnoredLexemeAction(createLexicalAnalyzerContext()); }

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

":enter"                                            { return AliasStateAction(createLexicalAnalyzerContext(), ENTER_ALIAS); }
":leave"                                            { return AliasStateAction(createLexicalAnalyzerContext(), LEAVE_ALIAS); }
":increment"                                        { return AliasStateAction(createLexicalAnalyzerContext(), INCREMENT_ALIAS); }
":decrement"                                        { return AliasStateAction(createLexicalAnalyzerContext(), DECREMENT_ALIAS); }

"=>"                                                { return PunctuationAction(createLexicalAnalyzerContext(), FORWARD_TRANSITION); }
"<=>"                                               { return PunctuationAction(createLexicalAnalyzerContext(), BIDIRECTIONAL_TRANSITION); }

"("                                                 { return PunctuationAction(createLexicalAnalyzerContext(), OPEN_PARENTHESIS); }
")"                                                 { return PunctuationAction(createLexicalAnalyzerContext(), CLOSE_PARENTHESIS); }

"["                                                 { return PunctuationAction(createLexicalAnalyzerContext(), OPEN_BRACKET); }
"]"                                                 { return PunctuationAction(createLexicalAnalyzerContext(), CLOSE_BRACKET); }

"{"                                                 { return PunctuationAction(createLexicalAnalyzerContext(), OPEN_BRACE); }
"}"                                                 { return PunctuationAction(createLexicalAnalyzerContext(), CLOSE_BRACE); }

","                                                 { return PunctuationAction(createLexicalAnalyzerContext(), COMMA); }
":"                                                 { return PunctuationAction(createLexicalAnalyzerContext(), COLON); }

"\'"                                                { return StringLiteralAction(createLexicalAnalyzerContext(), APOSTROPHE); }
"\""                                                { return StringLiteralAction(createLexicalAnalyzerContext(), QUOTATION_MARK); }

{EASING}                                            { return AnimationAction(createLexicalAnalyzerContext()); }

{NUMBER}{VALUE_UNIT}                                { return ValueAction(createLexicalAnalyzerContext()); }
{NUMBER}{TIME_UNIT}                                 { return TimeValueAction(createLexicalAnalyzerContext()); }
{COLOR}                                             { return ColorValueAction(createLexicalAnalyzerContext()); }

{ID}                                                { return StylePropertyAction(createLexicalAnalyzerContext()); }

{INTEGER}                                           { return IntegerAction(createLexicalAnalyzerContext()); }
{NUMBER}                                            { return FloatAction(createLexicalAnalyzerContext()); }

{WHITESPACE}+                                       { IgnoredLexemeAction(createLexicalAnalyzerContext()); }
.                                                   { return UnknownLexemeAction(createLexicalAnalyzerContext()); }

%%

#include "FlexExport.h"