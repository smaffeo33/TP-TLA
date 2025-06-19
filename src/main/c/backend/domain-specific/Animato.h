#ifndef ANIMATO_HEADER
#define ANIMATO_HEADER

/**
 * Este módulo valida la coherencia semántica del AST que describe
 * animaciones (triggers, estados, transiciones, keyframes, etc.).
 *
 * ► Devuelve un ComputationResult, reutilizando la misma estructura
 *   utilizada por Calculator para mantener uniformidad en todo el backend.
 *
 * ► Solo se considera que la validación fue satisfactoria cuando
 *   result.succeed == true.
 */

#include "../../frontend/syntactic-analysis/AbstractSyntaxTree.h"
#include "../../shared/Logger.h"
#include "../../shared/Type.h"
#include <limits.h>

typedef struct {
    boolean succeed;
    int value;
} ComputationResult;

typedef ComputationResult (*BinaryOperator)(const int, const int);


/** Inicializa el estado interno del validador semántico.  */
void initializeSemanticValidatorModule(void);

/** Libera los recursos y cierra el logger del validador semántico. */
void shutdownSemanticValidatorModule(void);


/**
 * Valida el AST completo del programa.
 *
 * @param program  Nodo raíz del AST (tipo Program*).
 *
 * @return ComputationResult
 *         • succeed == true  → no se hallaron errores semánticos.
 *         • succeed == false → value contiene el número de errores
 *           detectados durante la validación.
 */
ComputationResult validateProgram(Program * program);

#endif //ANIMATO_HEADER
