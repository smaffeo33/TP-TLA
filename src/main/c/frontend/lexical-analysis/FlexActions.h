/**
 * AngularTransitionActions.h
 * 
 * Defines action functions for the Angular transition syntax lexical analyzer.
 * These functions are called when specific patterns are matched in the flex file.
 */

#ifndef ANGULAR_TRANSITION_ACTIONS_H
#define ANGULAR_TRANSITION_ACTIONS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Enumeration of token types for Angular transition syntax
 */
typedef enum {
    // Special tokens
    TOKEN_EOF = 0,
    TOKEN_UNKNOWN,
    TOKEN_IGNORED,

    // Keywords
    TOKEN_TRIGGER,
    TOKEN_STATE,
    TOKEN_STYLE,
    TOKEN_TRANSITION,
    TOKEN_ANIMATE,
    TOKEN_SEQUENCE,
    TOKEN_GROUP,
    TOKEN_QUERY,
    TOKEN_STAGGER,
    TOKEN_KEYFRAMES,
    TOKEN_USE_ANIMATION,
    TOKEN_ANIMATION,

    // Special states
    TOKEN_VOID_STATE,
    TOKEN_WILDCARD_STATE,
    TOKEN_ENTER_QUERY,
    TOKEN_LEAVE_QUERY,

    // Transition operators
    TOKEN_FORWARD_TRANSITION,
    TOKEN_BIDIRECTIONAL_TRANSITION,

    // Delimiters and punctuation
    TOKEN_OPEN_PARENTHESIS,
    TOKEN_CLOSE_PARENTHESIS,
    TOKEN_OPEN_BRACKET,
    TOKEN_CLOSE_BRACKET,
    TOKEN_OPEN_BRACE,
    TOKEN_CLOSE_BRACE,
    TOKEN_COLON,
    TOKEN_COMMA,
    TOKEN_DOT,

    // Values
    TOKEN_IDENTIFIER,
    TOKEN_STRING_LITERAL,
    TOKEN_NUMBER,
    TOKEN_TIME_VALUE,
    TOKEN_EASING_FUNCTION,

    // Style-specific tokens
    TOKEN_STYLE_PROPERTY,
    TOKEN_STYLE_VALUE,
    TOKEN_STYLE_NUMBER,
    TOKEN_STYLE_UNIT
} TokenType;

/**
 * Enumeration for transition directions
 */
typedef enum {
    FORWARD_TRANSITION,
    BIDIRECTIONAL_TRANSITION
} TransitionDirection;

/**
 * Enumeration for parenthesis, bracket, and brace types
 */
typedef enum {
    OPEN_PARENTHESIS,
    CLOSE_PARENTHESIS,
    OPEN_BRACKET,
    CLOSE_BRACKET,
    OPEN_BRACE,
    CLOSE_BRACE
} DelimiterType;

/**
 * Structure to hold information about the current lexical context
 */
typedef struct {
    char* lexeme;       // The text that was matched
    int line;           // Current line number
    int column;         // Current column number
    int length;         // Length of the lexeme
} LexicalContext;

/**
 * Creates a new lexical context from the current match
 */
LexicalContext* createLexicalContext();

/**
 * Frees memory associated with a lexical context
 */
void destroyLexicalContext(LexicalContext* context);

/**
 * Action functions for each token type
 */

// Comment handling
void BeginMultilineCommentAction(LexicalContext* context);
void EndMultilineCommentAction(LexicalContext* context);

// Keyword actions
int TriggerKeywordAction(LexicalContext* context);
int StateKeywordAction(LexicalContext* context);
int StyleKeywordAction(LexicalContext* context);
int TransitionKeywordAction(LexicalContext* context);
int AnimateKeywordAction(LexicalContext* context);
int SequenceKeywordAction(LexicalContext* context);
int GroupKeywordAction(LexicalContext* context);
int QueryKeywordAction(LexicalContext* context);
int StaggerKeywordAction(LexicalContext* context);
int KeyframesKeywordAction(LexicalContext* context);
int UseAnimationKeywordAction(LexicalContext* context);
int AnimationKeywordAction(LexicalContext* context);

// Special state actions
int VoidStateAction(LexicalContext* context);
int WildcardStateAction(LexicalContext* context);
int EnterQueryAction(LexicalContext* context);
int LeaveQueryAction(LexicalContext* context);

// Transition operator actions
int TransitionArrowAction(LexicalContext* context, TransitionDirection direction);

// Delimiter actions
int ParenthesisAction(LexicalContext* context, DelimiterType type);
int BracketAction(LexicalContext* context, DelimiterType type);
int BraceAction(LexicalContext* context, DelimiterType type);
int ColonAction(LexicalContext* context);
int CommaAction(LexicalContext* context);
int DotAction(LexicalContext* context);

// Value actions
int IdentifierAction(LexicalContext* context);
int StringLiteralAction(LexicalContext* context);
int NumberAction(LexicalContext* context);
int TimeValueAction(LexicalContext* context);
int EasingFunctionAction(LexicalContext* context);

// Style-specific actions
int StylePropertyAction(LexicalContext* context);
int StyleValueAction(LexicalContext* context);
int StyleNumberAction(LexicalContext* context);
int StyleUnitAction(LexicalContext* context);

// Utility actions
int IgnoredLexemeAction(LexicalContext* context);
int UnknownLexemeAction(LexicalContext* context);

#endif /* ANGULAR_TRANSITION_ACTIONS_H */