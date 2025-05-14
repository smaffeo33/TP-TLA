#ifndef FLEX_ACTIONS_HEADER
#define FLEX_ACTIONS_HEADER

#include "../../shared/Environment.h"
#include "../../shared/Logger.h"
#include "../../shared/String.h"
#include "../../shared/Type.h"
#include "../syntactic-analysis/AbstractSyntaxTree.h"
#include "../syntactic-analysis/BisonParser.h"
#include "LexicalAnalyzerContext.h"
#include <stdio.h>
#include <stdlib.h>

/** Initialize module's internal state. */
void initializeFlexActionsModule();

/** Shutdown module's internal state. */
void shutdownFlexActionsModule();

/**
 * Flex lexeme processing actions.
 */

/* Comment handling */
void BeginMultilineCommentAction(LexicalAnalyzerContext * lexicalAnalyzerContext);
void EndMultilineCommentAction(LexicalAnalyzerContext * lexicalAnalyzerContext);
void IgnoredLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);

/* Keywords */
Token KeywordLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext, Token token);

/* State specifiers */
Token VoidStateAction(LexicalAnalyzerContext * lexicalAnalyzerContext);
Token WildcardStateAction(LexicalAnalyzerContext * lexicalAnalyzerContext);
Token EnterQueryAction(LexicalAnalyzerContext * lexicalAnalyzerContext);
Token LeaveQueryAction(LexicalAnalyzerContext * lexicalAnalyzerContext);

/* Transition arrows */
Token TransitionArrowAction(LexicalAnalyzerContext * lexicalAnalyzerContext, Token token);

/* Delimiters */
Token DelimiterAction(LexicalAnalyzerContext * lexicalAnalyzerContext, Token token);

/* Style object */
Token StylePropertyAction(LexicalAnalyzerContext * lexicalAnalyzerContext);
Token StyleValueAction(LexicalAnalyzerContext * lexicalAnalyzerContext);
Token StyleNumberAction(LexicalAnalyzerContext * lexicalAnalyzerContext);
Token StyleUnitAction(LexicalAnalyzerContext * lexicalAnalyzerContext);

/* Animation parameters */
Token TimeValueAction(LexicalAnalyzerContext * lexicalAnalyzerContext);
Token EasingFunctionAction(LexicalAnalyzerContext * lexicalAnalyzerContext);

/* Basic tokens */
Token StringLiteralAction(LexicalAnalyzerContext * lexicalAnalyzerContext);
Token IdentifierAction(LexicalAnalyzerContext * lexicalAnalyzerContext);
Token NumberAction(LexicalAnalyzerContext * lexicalAnalyzerContext);

/* Punctuation */
Token CommaAction(LexicalAnalyzerContext * lexicalAnalyzerContext);
Token ColonAction(LexicalAnalyzerContext * lexicalAnalyzerContext);
Token DotAction(LexicalAnalyzerContext * lexicalAnalyzerContext);

/* Error handling */
Token UnknownLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);

#endif