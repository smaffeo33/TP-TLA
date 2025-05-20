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
 * Create a lexical context from current position in the input.
 */
LexicalAnalyzerContext * createLexicalContext();

/**
 * Flex lexeme processing actions.
 */

/* Comment handling */
void BeginMultilineCommentLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);
void EndMultilineCommentLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);
void IgnoredLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);

/* Keywords */
Token KeywordLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext, Token token);

/* State handling */
Token StateAction(LexicalAnalyzerContext * lexicalAnalyzerContext, Token token);

/* Animation handling */
Token AnimationAction(LexicalAnalyzerContext * lexicalAnalyzerContext);

/* Style properties */
Token NameAction(LexicalAnalyzerContext * lexicalAnalyzerContext);

Token StylePropertySemanticAction(LexicalAnalyzerContext * lexicalAnalyzerContext, Token token);

Token StringLiteralAction(LexicalAnalyzerContext * lexicalAnalyzerContext, Token token);

Token StringValueAction(LexicalAnalyzerContext * lexicalAnalyzerContext);

Token AliasStateAction(LexicalAnalyzerContext * lexicalAnalyzerContext, Token token);

/* Punctuation */
Token PunctuationAction(LexicalAnalyzerContext * lexicalAnalyzerContext, Token token);

/* Unit handling */
Token ValueAction(LexicalAnalyzerContext * lexicalAnalyzerContext);
Token TimeValueAction(LexicalAnalyzerContext * lexicalAnalyzerContext);
Token ColorValueAction(LexicalAnalyzerContext * lexicalAnalyzerContext);


Token SelectorAction(LexicalAnalyzerContext * lexicalAnalyzerContext, Token token);

Token FloatAction(LexicalAnalyzerContext * lexicalAnalyzerContext);

/* Error handling */
Token UnknownLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);

#endif