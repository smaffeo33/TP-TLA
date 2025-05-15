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
WHITESPACE  [ \t\r\n]
DIGIT       [0-9]
ALPHA       [a-zA-Z]
ALPHANUM    [a-zA-Z0-9]
ID          [a-zA-Z][a-zA-Z0-9_]*
STRING      ['"][^'"]*['"]
NUMBER      {DIGIT}+(\.{DIGIT}+)?
INTEGER     {DIGIT}+
TIME_UNIT   "ms"|"s"
COLOR       (#[0-9a-fA-F]{6}|#[0-9a-fA-F]{3}|rgba\([ \t\r\n]*{NUMBER}[ \t\r\n]*,[ \t\r\n]*{NUMBER}[ \t\r\n]*,[ \t\r\n]*{NUMBER}[ \t\r\n]*,[ \t\r\n]*{NUMBER}[ \t\r\n]*\)|rgb\([ \t\r\n]*{NUMBER}[ \t\r\n]*,[ \t\r\n]*{NUMBER}[ \t\r\n]*,[ \t\r\n]*{NUMBER}[ \t\r\n]*\))
VALUE_UNIT  "px"|"em"|"%"|"rem"|"vh"|"vw"
EASING      "ease"|"ease-in"|"ease-out"|"ease-in-out"|"linear"|"cubic-bezier"{WHITESPACE}*\({WHITESPACE}*{NUMBER}{WHITESPACE}*,{WHITESPACE}*{NUMBER}{WHITESPACE}*,{WHITESPACE}*{NUMBER}{WHITESPACE}*,{WHITESPACE}*{NUMBER}{WHITESPACE}*\)


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

"\'"                                                { return StringLiteralAction(createLexicalContext(), APOSTROPHE); }
"\""                                                { return StringLiteralAction(createLexicalContext(), QUOTATION_MARK); }

{EASING}                                            { return AnimationAction(createLexicalContext(), EASING); }

{NUMBER}{VALUE_UNIT}                                { return ValueAction(createLexicalContext(), VALUE); }
{NUMBER}{TIME_UNIT}                                 { return ValueAction(createLexicalContext(), TIME); }

{NUMBER}                                            { return ValueAction(createLexicalContext(), NUMBER); }
{COLOR}                                             { return ValueAction(createLexicalContext(), COLOR_VALUE); }

{ID}                                                { return StylePropertyAction(createLexicalContext()); }

{INTEGER}                                           { return IntegerAction(createLexicalContext()); }

{WHITESPACE}+                                       { IgnoredLexemeAction(createLexicalContext()); }
.                                                   { return UnknownLexemeAction(createLexicalContext()); }

%%

#include "FlexExport.h"