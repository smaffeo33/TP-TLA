#ifndef SYMBOL_TABLE_HEADER
#define SYMBOL_TABLE_HEADER

#include <stdbool.h>
#include <stddef.h>

#define MAX_SYMBOLS 1024

typedef enum {
    SYMBOL_TRIGGER,
    SYMBOL_STATE,
    SYMBOL_TRANSITION,
    SYMBOL_ANIMATE,
    SYMBOL_KEYFRAME,
    SYMBOL_PROPERTY
} SymbolKind;

typedef struct {
    char *name;
    SymbolKind kind;
} Symbol;

typedef struct {
    Symbol symbols[MAX_SYMBOLS];
    size_t count;
} SymbolTable;

/* Inicializa la tabla */
void stInit(SymbolTable *table);

/**
 * Inserta un nombre.
 * @return true  si el nombre NO existía y se agregó con éxito.
 *         false si el nombre ya estaba presente (duplicado).
 */
bool stAdd(SymbolTable *table, const char *name, SymbolKind kind);

/**
 * Busca un nombre.
 * @return true  si ya existe en la tabla.
 *         false si no se encontró.
 */
bool stContains(const SymbolTable *table, const char *name);

/* Libera memoria asociada a la tabla */
void stDestroy(SymbolTable *table);

#endif /* SYMBOL_TABLE_HEADER */
