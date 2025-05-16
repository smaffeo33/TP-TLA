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
		releaseTriggerBlock(trigger->block);
		free(trigger);
	}
}

void releaseTriggerBlock(TriggerBlock * triggerBlock) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (triggerBlock != NULL) {
		releaseTransitionList(triggerBlock->transitionList);
		releaseStateList(triggerBlock->stateList);
		free(triggerBlock);
	}
}

void releaseStateList(StateList * stateList) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (stateList != NULL) {
		for (size_t i = 0; i < stateList->stateCount; i++) {
			releaseState(stateList->states[i]);
		}
		free(stateList->states);
		free(stateList);
	}
}

void releaseTransitionList(TransitionList * transitionList) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (transitionList != NULL) {
		for (size_t i = 0; i < transitionList->transitionCount; i++) {
			releaseTransition(transitionList->transitions[i]);
		}
		free(transitionList->transitions);
		free(transitionList);
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
        releaseTransitionRule(transition->transitionRule);
        releaseTransitionBlock(transition->transitionBlock);
		free(transition);
	}
}

void releaseTransitionRule(TransitionRule * transitionRule) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (transitionRule != NULL) {
        free(transitionRule->fromState);
        free(transitionRule->toState);
        free(transitionRule);
    }
}

void releaseTransitionBlock(TransitionBlock * transitionBlock) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (transitionBlock != NULL) {
        releaseAnimate(transitionBlock->animate);
        releaseStyle(transitionBlock->style);
        free(transitionBlock);
    }
}

void releaseStyle(Style * style) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (style != NULL) {
		releasePropertyList(style->properties);
		free(style);
	}
}

void releasePropertyList(PropertyList * propertyList) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (propertyList != NULL) {
		for (size_t i = 0; i < propertyList->propertyCount; i++) {
			releaseProperty(propertyList->properties[i]);
		}
		free(propertyList->properties);
		free(propertyList);
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
        switch (property->type) {
            case STRING:
                free(property->value);
                break;
            case FLOAT:
                break;
            case INT:
                break;
        }
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
