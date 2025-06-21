#ifndef SYMBOL_TABLE
#define SYMBOL_TABLE

#include <stddef.h>

#include "Type.h"

#define MAX_SYMBOLS 1024

struct SymbolTable;

typedef enum SymbolKind {
    SYMBOL_UNKNOWN = -1,
    SYMBOL_TRIGGER,
    SYMBOL_STATE,
    SYMBOL_TRANSITION,
    SYMBOL_ANIMATE,
    SYMBOL_KEYFRAME,
    SYMBOL_PROPERTY
} SymbolKind;

// typedef enum {
//     STEP_ANIMATE,
//     STEP_QUERY,
//     STEP_GROUP,
//     STEP_SEQUENCE,
//     STEP_STAGGER
// } StepItemKind;

typedef struct {
    char *name;
    SymbolKind kind;
    struct  SymbolTable *subTable; // solo usado si es un trigger
} Symbol;

typedef struct SymbolTable {
    Symbol symbols[MAX_SYMBOLS];
    size_t count;
} SymbolTable;


void stInit(SymbolTable *table);

/**
 * Inserta un nombre.
 * @return true  si el nombre NO existía y se agregó con éxito.
 *         false si el nombre ya estaba presente (duplicado).
 */
boolean stAdd(SymbolTable *table, const char *name, SymbolKind kind);

/**
 * Busca un nombre.
 * @return true  si ya existe en la tabla.
 *         false si no se encontró.
 */
boolean stContains(const SymbolTable *table, const char *name);

void stDestroy(SymbolTable *table);

Symbol* stGet(SymbolTable *table, const char *name);

#endif /* SYMBOL_TABLE_HEADER */
