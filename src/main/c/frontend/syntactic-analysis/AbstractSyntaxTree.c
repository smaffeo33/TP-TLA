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

void releaseGroup(Group * group) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (group != NULL) {
        releaseTransitionBlockItemList(group->transitionBlockItemList);
        free(group);
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
        switch (transitionRule->ruleType) {
            case FROM_TO:
                free(transitionRule->fromState);
                free(transitionRule->toState);
                break;
            case ALIAS:
                break;
        }
        free(transitionRule);
    }
}

void releaseTransitionBlock(TransitionBlock * transitionBlock) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (transitionBlock != NULL) {
        releaseTransitionBlockItemList(transitionBlock->transitionBlockItemList);
        free(transitionBlock);
    }
}

void releaseTransitionBlockItemList(TransitionBlockItemList * transitionBlockItemList) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (transitionBlockItemList != NULL) {
        for (size_t i = 0; i < transitionBlockItemList->itemCount; i++) {
            releaseTransitionBlockItem(transitionBlockItemList->items[i]);
        }
        free(transitionBlockItemList->items);
        free(transitionBlockItemList);
    }
}

void releaseTransitionBlockItem(TransitionBlockItem * transitionBlockItem) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (transitionBlockItem != NULL) {
        switch (transitionBlockItem->type) {
            case ANIMATE_ITEM:
                releaseAnimate((Animate *) transitionBlockItem->item);
                break;
            case STYLE_ITEM:
                releaseStyle((Style *) transitionBlockItem->item);
                break;
        }
        free(transitionBlockItem);
    }
}

void releaseStyle(Style * style) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (style != NULL) {
		releasePropertyList(style->properties);
		free(style);
	}
}

void releaseKeyframeStyle(KeyframeStyle * keyframeStyle) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (keyframeStyle != NULL) {
        releasePropertyList(keyframeStyle->properties);
        free(keyframeStyle);
    }
}

void releaseKeyframeStyleList(KeyframeStyleList * keyframeStyleList) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (keyframeStyleList != NULL) {
        for (size_t i = 0; i < keyframeStyleList->keyframeCount; i++) {
            releaseKeyframeStyle(keyframeStyleList->keyframeStyles[i]);
        }
        free(keyframeStyleList->keyframeStyles);
        free(keyframeStyleList);
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

void releaseKeyframes(Keyframes * keyframes) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (keyframes != NULL) {
        releaseKeyframeStyleList(keyframes->keyframeStyleList);
        free(keyframes);
    }
}

void releaseAnimate(Animate * animate) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (animate != NULL) {
		free(animate->duration);
		free(animate->easing);
        switch (animate->type) {
            case ANIMATE_WITH_KEYFRAMES:
                releaseKeyframes(animate->keyframes);
                break;
            case ANIMATE_WITH_STYLE:
                releaseStyle(animate->style);
                break;
            case NONE:
                break;
        }
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
            default:
                break;
        }
		free(property);
	}
}

void releaseSequence(Sequence * sequence) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (sequence != NULL) {
        releaseTransitionBlockItemList(sequence->transitionBlockItemList);
        free(sequence);
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

void releaseStagger(Stagger * stagger) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (stagger != NULL) {
        free(stagger->time);
        releaseTransitionBlockItemList(stagger->transitionBlockItemList);
        free(stagger);
    }
}
