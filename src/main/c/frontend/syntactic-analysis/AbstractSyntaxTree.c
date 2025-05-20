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
        releaseStepItemList(group->stepItemList);
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
        releaseStepItemList(transitionBlock->stepItemList);
        free(transitionBlock);
    }
}

void releaseStepItemList(StepItemList * stepItemList) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (stepItemList != NULL) {
        for (size_t i = 0; i < stepItemList->itemCount; i++) {
            releaseStepItem(stepItemList->items[i]);
        }
        free(stepItemList->items);
        free(stepItemList);
    }
}

void releaseStepItem(StepItem * stepItem) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (stepItem != NULL) {
        switch (stepItem->type) {
            case ANIMATE_ITEM:
                releaseAnimate((Animate *) stepItem->item);
                break;
            case STYLE_ITEM:
                releaseStyle((Style *) stepItem->item);
                break;
            case GROUP_ITEM:
                releaseGroup((Group *) stepItem->item);
                break;
            case SEQUENCE_ITEM:
                releaseSequence((Sequence *) stepItem->item);
                break;
            case QUERY_ITEM:
                releaseQuery((Query *) stepItem->item);
                break;
            case STAGGER_ITEM:
                releaseStagger((Stagger *) stepItem->item);
                break;
        }
        free(stepItem);
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
        releaseAnimateInfo(animate->animateInfo);
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

void releaseAnimateInfo(AnimateInfo * animateInfo) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (animateInfo != NULL) {
        switch (animateInfo->type) {
            case DURATION_DELAY:
                free(animateInfo->delay);
                break;
            case DURATION_EASING:
                free(animateInfo->easing);
                break;
            case DURATION_DELAY_EASING:
                free(animateInfo->delay);
                free(animateInfo->easing);
                break;
        }
        free(animateInfo->duration);
        free(animateInfo);
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
        releaseStepItemList(sequence->stepItemList);
        free(sequence);
    }
}

void releaseQuery(Query * query) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (query != NULL) {
        switch (query->selectorType) {
            case ALIAS_SELECTOR:
                break;
            default:
                free(query->selector);
        }
        releaseStepItemList(query->stepItemList);
        free(query);
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
        releaseStepItemList(stagger->stepItemList);
        free(stagger);
    }
}
