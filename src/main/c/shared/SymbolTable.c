#include "SymbolTable.h"
#include <string.h>
#include <stdlib.h>


boolean stAdd(SymbolTable *table, const char *name, SymbolKind kind){
    if (table->count >= MAX_SYMBOLS) {
        return false; // No space left in the symbol table
    }

    for (size_t i = 0; i < table->count; ++i) {
        if (strcmp(table->symbols[i].name, name) == 0) {
            return false; // Symbol already exists
        }
    }

    table->symbols[table->count].name = strdup(name);
    table->symbols[table->count].kind = kind;
    table->symbols[table->count].subTable = NULL; // Initialize subTable to NULL
    table->count++;

    return true; // Successfully added the symbol
}

void stInit(SymbolTable *table) {
    if(!table){
        return;
    }
    table->count = 0;
    for (size_t i = 0; i < MAX_SYMBOLS; ++i) {
        table->symbols[i].name = NULL; // Initialize names to NULL
        table->symbols[i].kind = SYMBOL_UNKNOWN; // -1
        table->symbols[i].subTable = NULL; // Initialize subTable to NULL
    }
}
void stDestroy(SymbolTable *table) {
    for (size_t i = 0; i < table->count; ++i) {
        free(table->symbols[i].name); // Free the name string
        if (table->symbols[i].subTable != NULL) {
            stDestroy(table->symbols[i].subTable); // Recursively destroy subTables
            free(table->symbols[i].subTable); // Free the subTable pointer
        }
    }
    table->count = 0; // Reset count after destruction
}

Symbol* stGet(SymbolTable *table, const char *name) {
    for (size_t i = 0; i < table->count; ++i) {
        if (strcmp(table->symbols[i].name, name) == 0) {
            return &table->symbols[i];
        }
    }
    return NULL;
}

boolean stContains(const SymbolTable *table, const char *name) {
    for (size_t i = 0; i < table->count; ++i) {
        if (strcmp(table->symbols[i].name, name) == 0) {
            return true; // Symbol found
        }
    }
    return false; // Symbol not found
}
