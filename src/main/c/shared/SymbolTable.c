#include "SymbolTable.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void stInit(SymbolTable *table) {
    table->count = 0;
}

bool stAdd(SymbolTable *table, const char *name, SymbolKind kind) {
    if (!table || !name) return false;

    // Buscar duplicado
    for (size_t i = 0; i < table->count; ++i) {
        if (strcmp(table->symbols[i].name, name) == 0) {
            return false;  // Ya existe
        }
    }

    if (table->count >= MAX_SYMBOLS) {
        fprintf(stderr, "SymbolTable overflow: max %d symbols\n", MAX_SYMBOLS);
        return false;
    }

    // Agregar nuevo símbolo
    Symbol *sym = &table->symbols[table->count++];
    sym->name = strdup(name);  // Copiar string
    sym->kind = kind;

    return true;
}

bool stContains(const SymbolTable *table, const char *name) {
    if (!table || !name) return false;

    for (size_t i = 0; i < table->count; ++i) {
        if (strcmp(table->symbols[i].name, name) == 0) {
            return true;
        }
    }

    return false;
}

void stDestroy(SymbolTable *table) {
    if (!table) return;

    for (size_t i = 0; i < table->count; ++i) {
        free(table->symbols[i].name);
    }
    table->count = 0;
}
