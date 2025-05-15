#include "AbstractSyntaxTree.h"

/* MODULE INTERNAL STATE */

static Logger * _logger = NULL;

void initializeAbstractSyntaxTreeModule() {
	_logger = createLogger("AbstractSyntxTree");
}

void shutdownAbstractSyntaxTreeModule() {
	if (_logger != NULL) {
		destroyLogger(_logger);
	}
}

/** PUBLIC FUNCTIONS */

void releaseTrigger(Trigger * trigger) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (trigger != NULL) {
		free(trigger->name);
		for (size_t i = 0; i < trigger->stateCount; i++) {
			releaseState(trigger->states[i]);
		}
		free(trigger->states);
		for (size_t i = 0; i < trigger->transitionCount; i++) {
			releaseTransition(trigger->transitions[i]);
		}
	}
}

void releaseState(State * state) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (state != NULL) {
		free(state->name);
		releaseStyle(state->style);
		free(state);
	}
}

void releaseTransition(Transition * transition) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (transition != NULL) {
		free(transition->fromState);
		free(transition->toState);
		releaseAnimate(transition->animate);
		releaseStyle(transition->style);
		free(transition);
	}
}

void releaseStyle(Style * style) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (style != NULL) {
		for (size_t i = 0; i < style->propertyCount; i++) {
			releaseProperty(style->properties[i]);
		}
		free(style->properties);
		free(style);
	}
}

void releaseAnimate(Animate * animate) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (animate != NULL) {
		free(animate->duration);
		free(animate->easing);
		free(animate);
	}
}

void releaseProperty(Property * property) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (property != NULL) {
		free(property->name);
		free(property->value);
		free(property);
	}
}

void releaseTriggerList(TriggerList * triggerList) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (triggerList != NULL) {
		for (size_t i = 0; i < triggerList->triggerCount; i++) {
			releaseTrigger(triggerList->trigger[i]);
		}
		free(triggerList->trigger);
		free(triggerList);
	}
}

void releaseProgram(Program * program) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (program != NULL) {
		releaseTriggerList(program->triggerList);
		free(program);
	}
}
