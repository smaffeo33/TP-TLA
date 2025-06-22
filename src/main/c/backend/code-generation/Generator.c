#include "Generator.h"
#include "../../frontend/syntactic-analysis/AbstractSyntaxTree.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

static Logger   *_logger     = NULL;
static const char _indentChar = ' ';
static const unsigned _indentSize = 4;
static unsigned _uniqueId    = 0;

static void   _printIndent (FILE *f, unsigned lvl);
static void   _out         (FILE *f, unsigned lvl, const char *fmt, ...);
static const char *_sanitize    (const char *name);
static const char *_aliasToState(AliasType a);
static void   _genProgramCss   (FILE *f, Program *p);
static void   _genTriggerCss   (FILE *f, Trigger *t);
static Style *_findFirstStyleItem(StepItemList *list);
static StepItem *_findFirstAnimateItem(StepItemList *list);
static void _extractAnimationProps(Animate *a, char **propDuration, char **propEasing, char **propDelay);
static void   _genStateClassCss(FILE *f, const char *triggerName, State *s, TriggerBlock *block);
static void   _genStylePropsCss(FILE *f, unsigned lvl, PropertyList *plist);
static void   _genStepItem     (FILE *f, const char *prefix, StepItem *step);
static void   _genTransitionCss(FILE *f, Trigger *t, Transition *tr);
static void   _genAnimateCss   (FILE *f, const char *prefix, Animate *a,
                                Style *fromStyle, Style *toStyle);

void initializeGeneratorModule(void)  { _logger = createLogger("Generator"); }
void shutdownGeneratorModule(void)    { destroyLogger(_logger);            }

static const char *_normalizeSelectorName(const char *sel) {
    static char buf[128];
    size_t j = 0;
    for (size_t i = 0; sel && sel[i] && j < sizeof(buf) - 1; ++i) {
        if (sel[i] != '.' && sel[i] != '#') buf[j++] = sel[i];
    }
    buf[j] = '\0';
    return buf;
}

void generate(CompilerState *compilerState)
{
    if (!compilerState || !compilerState->abstractSyntaxtTree) {
        logError(_logger, "Nothing to generate (NULL AST).");
        return;
    }

    _uniqueId = 0;

    logDebugging(_logger, "Generating CSS…");
    _genProgramCss(stdout, compilerState->abstractSyntaxtTree);
}

static void _printIndent(FILE *f, unsigned lvl)
{
    for (unsigned i = 0; i < lvl * _indentSize; ++i)
        fputc(_indentChar, f);
}

static void _out(FILE *f, unsigned lvl, const char *fmt, ...)
{
    _printIndent(f, lvl);

    va_list ap; va_start(ap, fmt);
    vfprintf(f, fmt, ap);
    va_end(ap);

    size_t len = strlen(fmt);
    if (len == 0 || fmt[len - 1] != '\n')
        fputc('\n', f);
}

static const char *_sanitize(const char *name)
{
    if (!name)      return "";
    if (strcmp(name, "*")   == 0) return "active";
    if (strcmp(name, "void") == 0) return "void";
    return name;
}

static const char *_aliasToState(AliasType a)
{
    switch (a) {
        case ENTER:     return "enter";
        case LEAVE:     return "leave";
        case INCREMENT: return "increment";
        case DECREMENT: return "decrement";
        default:        return "error";
    }
}

static void _genStylePropsCss(FILE *f, unsigned lvl, PropertyList *plist)
{
    if (!plist) return;

    // Transform properties need to to be combined
    char transformValue[256] = {0};
    int hasTransform = 0;

    for (size_t i = 0; i < plist->propertyCount; ++i) {
        Property *p = (plist->properties) ? plist->properties[i] : NULL;
        if (!p || !p->name) continue;

        const char *name = p->name;

        char val[64] = {0};
        switch (p->type) {
            case UNITLESS:
                snprintf(val, sizeof val, "%g", p->floatValue);
                break;
            case UNIT:
            case STRING:
            case COLOR:
                if (p->value && *p->value)
                    strncpy(val, p->value, sizeof val - 1);
                else
                    snprintf(val, sizeof val, "%g", p->floatValue);
                break;
            default:
                break;
        }

        // Handle transform properties by collecting them
        if (strcmp(name, "scale") == 0) {
            if (!*val) strcpy(val, "1");
            char transform[64];
            snprintf(transform, sizeof transform, "scale(%s) ", val);
            strncat(transformValue, transform, sizeof(transformValue) - strlen(transformValue) - 1);
            hasTransform = 1;
            continue;
        }
        if (strcmp(name, "translateY") == 0 || strcmp(name, "translateX") == 0) {
            const char *func = (strcmp(name, "translateY") == 0) ? "translateY" : "translateX";
            char transform[64];
            snprintf(transform, sizeof transform, "%s(%s) ", func, val);
            strncat(transformValue, transform, sizeof(transformValue) - strlen(transformValue) - 1);
            hasTransform = 1;
            continue;
        }
        if (strcmp(name, "rotate") == 0) {
            char transform[64];
            snprintf(transform, sizeof transform, "rotate(%s) ", val);
            strncat(transformValue, transform, sizeof(transformValue) - strlen(transformValue) - 1);
            hasTransform = 1;
            continue;
        }

        if (*val) {
            _out(f, lvl, "%s: %s;", name, val);
        }
    }

    // Output combined transform property if we have any
    if (hasTransform) {
        // Trim trailing space
        size_t len = strlen(transformValue);
        if (len > 0 && transformValue[len-1] == ' ') {
            transformValue[len-1] = '\0';
        }
        _out(f, lvl, "transform: %s;", transformValue);
    }
}

static void _genStateClassCss(FILE *f, const char *triggerName, State *s, TriggerBlock *block)
{
    const char *san = _sanitize(s->name);
    if (strcmp(san, "void") == 0) return;

    // Transition properties that will be applied to this state
    char transitionProps[512] = {0};
    char transitionDuration[64] = "0s";
    char transitionEasing[64] = "ease";
    int hasTransition = 0;

    // Collect transition properties if this state is part of a transition
    if (block && block->transitionList) {
        for (size_t i = 0; i < block->transitionList->transitionCount; ++i) {
            Transition *tr = block->transitionList->transitions[i];
            if (!tr || !tr->transitionRule || !tr->transitionBlock) continue;

            int isPartOfTransition = 0;
            int isFromState = 0; // Flag to indicate if this state is the "from" state in the transition
            const char *fromState = NULL, *toState = NULL;
            char *transitionDurationForState = NULL;
            char *transitionEasingForState = NULL;

            if (tr->transitionRule->ruleType == FROM_TO) {
                fromState = tr->transitionRule->fromState;
                toState = tr->transitionRule->toState;

                // Check if this state appears as the "to" state in a forward transition
                if (toState && strcmp(s->name, toState) == 0 && tr->transitionRule->direction == FORWARD) {
                    isPartOfTransition = 1;
                    isFromState = 1;

                    // Extract animation properties for when this state is the "from" state in the reverse direction
                    StepItemList *sil = tr->transitionBlock->stepItemList;
                    for (size_t j = 0; j < sil->itemCount; ++j) {
                        StepItem *si = sil->items[j];
                        if (!si || si->type != ANIMATE_ITEM) continue;

                        Animate *a = (Animate *)si->item;
                        char *duration, *easing, *delay;
                        _extractAnimationProps(a, &duration, &easing, &delay);
                        transitionDurationForState = duration;
                        transitionEasingForState = easing;
                        break;
                    }
                } else if (fromState && toState && tr->transitionRule->direction == BIDIRECTIONAL) {
                    // For bidirectional transitions, check if this state is either the "from" or "to" state
                    if (strcmp(s->name, fromState) == 0 || strcmp(s->name, toState) == 0) {
                        isPartOfTransition = 1;
                        isFromState = 1; // In bidirectional, the "to" state is also a "from" state for the reverse direction

                        // Extract animation properties for transitions involving this state
                        StepItemList *sil = tr->transitionBlock->stepItemList;
                        for (size_t j = 0; j < sil->itemCount; ++j) {
                            StepItem *si = sil->items[j];
                            if (!si || si->type != ANIMATE_ITEM) continue;

                            Animate *a = (Animate *)si->item;
                            char *duration, *easing, *delay;
                            _extractAnimationProps(a, &duration, &easing, &delay);
                            transitionDurationForState = duration;
                            transitionEasingForState = easing;
                            break;
                        }
                    }
                }
            } else if (tr->transitionRule->ruleType == ALIAS) {
                // For aliases like ENTER, LEAVE, etc.
                if (tr->transitionRule->alias == ENTER &&
                    (strcmp(s->name, "void") == 0)) { // Only "void" is the "from" state in ENTER
                    isPartOfTransition = 1;
                    isFromState = 1;
                } else if (tr->transitionRule->alias == LEAVE &&
                           (strcmp(s->name, "*") == 0 || strcmp(s->name, "active") == 0)) {
                    isPartOfTransition = 1;
                    isFromState = 1;
                }
            }

            if (isPartOfTransition && isFromState && transitionDurationForState && transitionEasingForState) {
                // Set the duration and easing for the transition
                strncpy(transitionDuration, transitionDurationForState, sizeof(transitionDuration) - 1);
                strncpy(transitionEasing, transitionEasingForState, sizeof(transitionEasing) - 1);

                // Extract the properties to transition
                if (s->style && s->style->properties) {
                    PropertyList *plist = s->style->properties;
                    for (size_t k = 0; k < plist->propertyCount; ++k) {
                        Property *p = plist->properties[k];
                        if (!p || !p->name) continue;

                        // Skip transform properties as they need special handling
                        if (strcmp(p->name, "scale") == 0 ||
                            strcmp(p->name, "translateX") == 0 ||
                            strcmp(p->name, "translateY") == 0 ||
                            strcmp(p->name, "rotate") == 0) {
                            continue;
                        }

                        if (hasTransition) {
                            strncat(transitionProps, ", ", sizeof(transitionProps) - strlen(transitionProps) - 1);
                        }
                        strncat(transitionProps, p->name, sizeof(transitionProps) - strlen(transitionProps) - 1);
                        strncat(transitionProps, " ", sizeof(transitionProps) - strlen(transitionProps) - 1);
                        strncat(transitionProps, transitionDuration, sizeof(transitionProps) - strlen(transitionProps) - 1);
                        strncat(transitionProps, " ", sizeof(transitionProps) - strlen(transitionProps) - 1);
                        strncat(transitionProps, transitionEasing, sizeof(transitionProps) - strlen(transitionProps) - 1);
                        hasTransition = 1;
                    }

                    // Handle transform properties if any
                    int hasTransformProp = 0;
                    for (size_t k = 0; k < plist->propertyCount; ++k) {
                        Property *p = plist->properties[k];
                        if (!p || !p->name) continue;

                        if (strcmp(p->name, "scale") == 0 ||
                            strcmp(p->name, "translateX") == 0 ||
                            strcmp(p->name, "translateY") == 0 ||
                            strcmp(p->name, "rotate") == 0) {
                            hasTransformProp = 1;
                            break;
                        }
                    }

                    if (hasTransformProp) {
                        if (hasTransition) {
                            strncat(transitionProps, ", ", sizeof(transitionProps) - strlen(transitionProps) - 1);
                        }
                        strncat(transitionProps, "transform", sizeof(transitionProps) - strlen(transitionProps) - 1);
                        hasTransition = 1;
                    }
                }
                break; // Only use the first animate item
            }
        }
    }

    // Add a comment for the state
    _out(f, 0, "/* %s state */", san);

    // Generate base state class with transitions
    _out(f, 0, ".%s.%s {", triggerName, san);
    _genStylePropsCss(f, 1, s->style ? s->style->properties : NULL);

    // Add transition property if we found any animatable properties
    if (hasTransition) {
        _out(f, 1, "transition: %s;", transitionProps);
    }

    _out(f, 0, "}");
}

static void _genAnimateCss(FILE *f,
                           const char *prefix,
                           Animate    *a,
                           Style      *fromStyle,
                           Style      *toStyle)
{
    if (!a) return;

    AnimateInfo *ai = a->animateInfo;
    const char *dur = (ai && ai->duration) ? ai->duration : "0ms";
    const char *eas = (ai && ai->easing)   ? ai->easing   : "ease";
    const char *del = (ai && ai->delay)    ? ai->delay    : "0ms";
    int   hasDelay = (ai && ai->delay && strcmp(del, "0ms") != 0 && strcmp(del, "0") != 0);

    unsigned id = _uniqueId++;
    char kfName [128]; snprintf(kfName,  sizeof kfName,  "%s-kf-%u",   prefix, id);
    char clsName[256]; snprintf(clsName, sizeof clsName, "%s-anim-%u", prefix, id);

    PropertyList *destProps = NULL;
    if (toStyle) destProps = toStyle->properties;
    else if (a->type == ANIMATE_WITH_STYLE && a->style)
        destProps = a->style->properties;

    _out(f, 0, "@keyframes %s {", kfName);

    if (a->type == ANIMATE_WITH_KEYFRAMES && a->keyframes && a->keyframes->keyframeStyleList) {
        KeyframeStyleList *kl = a->keyframes->keyframeStyleList;
        for (size_t i = 0; i < kl->keyframeCount; ++i) {
            KeyframeStyle *ks = kl->keyframeStyles[i];
            float pct = (ks->offset <= 1.f) ? ks->offset * 100.f : ks->offset;
            _out(f, 1, "%.0f%% {", pct);
            _genStylePropsCss(f, 2, ks->properties);
            _out(f, 1, "}");
        }
    } else {
        _out(f, 1, "from {");
        if (fromStyle) {
            _genStylePropsCss(f, 2, fromStyle->properties);
        }
        _out(f, 1, "}");

        _out(f, 1, "to {");
        if (toStyle) {
            _genStylePropsCss(f, 2, toStyle->properties);
        } else if (a->type == ANIMATE_WITH_STYLE && a->style) {
            _genStylePropsCss(f, 2, a->style->properties);
        }
        _out(f, 1, "}");
    }

    _out(f, 0, "}");

    _out(f, 0, ".%s {", clsName);
    if (hasDelay) {
        _out(f, 1, "animation-name: %s;", kfName);
        _out(f, 1, "animation-duration: %s;", dur);
        _out(f, 1, "animation-timing-function: %s;", eas);
        _out(f, 1, "animation-delay: %s;", del);
        _out(f, 1, "animation-fill-mode: forwards;");
        _out(f, 1, "animation-direction: normal;");
    } else {
        _out(f, 1, "animation-name: %s;", kfName);
        _out(f, 1, "animation-duration: %s;", dur);
        _out(f, 1, "animation-timing-function: %s;", eas);
        _out(f, 1, "animation-fill-mode: forwards;");
        _out(f, 1, "animation-direction: normal;");
    }
    _out(f, 0, "}");
}

static void _genStepItem(FILE *f, const char *prefix, StepItem *step)
{
    if (!step) return;

    switch (step->type) {
        case ANIMATE_ITEM:
            _genAnimateCss(f, prefix, (Animate *)step->item, NULL, NULL);
            break;

        case STYLE_ITEM: {
            Style *st = (Style *)step->item;
            char tmp[64];
            snprintf(tmp, sizeof tmp, "%s-style-%u", prefix, _uniqueId++);
            _out(f, 0, ".%s {", tmp);
            _genStylePropsCss(f, 1, st ? st->properties : NULL);
            _out(f, 0, "}");
            break;
        }

        case GROUP_ITEM:
        case SEQUENCE_ITEM: {
            StepItemList *lst = (step->type == GROUP_ITEM)
                                ? ((Group *)step->item)->stepItemList
                                : ((Sequence *)step->item)->stepItemList;

            // Generate a unique class for this group/sequence
            char groupClass[256];
            snprintf(groupClass, sizeof groupClass, "%s-%s-%u",
                    prefix,
                    (step->type == GROUP_ITEM) ? "group" : "seq",
                    _uniqueId++);

            // For sequences, we need to handle timing differently
            if (step->type == SEQUENCE_ITEM) {
                float cumulativeDelay = 0.0;

                for (size_t i = 0; i < lst->itemCount; ++i) {
                    StepItem *item = lst->items[i];
                    if (!item) continue;

                    // Calculate sequence timing
                    char itemPrefix[256];
                    snprintf(itemPrefix, sizeof itemPrefix, "%s-item-%zu", groupClass, i);

                    // Create a delayed version of this item
                    if (cumulativeDelay > 0.0 && item->type == ANIMATE_ITEM) {
                        Animate *anim = (Animate *)item->item;
                        char delayStr[32];

                        // Add delay to animation if it exists
                        if (anim && anim->animateInfo) {
                            // Convert cumulative delay to ms or s based on existing format
                            if (anim->animateInfo->delay && strstr(anim->animateInfo->delay, "ms")) {
                                snprintf(delayStr, sizeof delayStr, "%.0fms", cumulativeDelay);
                            } else {
                                snprintf(delayStr, sizeof delayStr, "%.2fs", cumulativeDelay / 1000.0);
                            }
                            anim->animateInfo->delay = strdup(delayStr);
                        }
                    }

                    _genStepItem(f, itemPrefix, item);

                    // Increment cumulative delay for sequences
                    if (item->type == ANIMATE_ITEM) {
                        Animate *anim = (Animate *)item->item;
                        if (anim && anim->animateInfo && anim->animateInfo->duration) {
                            // Parse duration to add to cumulative delay
                            if (strstr(anim->animateInfo->duration, "ms")) {
                                cumulativeDelay += atof(anim->animateInfo->duration);
                            } else if (strstr(anim->animateInfo->duration, "s")) {
                                cumulativeDelay += atof(anim->animateInfo->duration) * 1000.0;
                            }
                        }
                    }
                }
            } else {
                // For groups, process all items in parallel
                for (size_t i = 0; i < lst->itemCount; ++i) {
                    char itemPrefix[256];
                    snprintf(itemPrefix, sizeof itemPrefix, "%s-item-%zu", groupClass, i);
                    _genStepItem(f, itemPrefix, lst->items[i]);
                }
            }
            break;
        }

        case QUERY_ITEM: {
            Query *q = (Query *)step->item;
            const char *sel = (q->selectorType == ALIAS_SELECTOR)
                              ? _sanitize(_aliasToState(q->alias))
                              : _normalizeSelectorName(q->selector);

            char queryPrefix[256];
            snprintf(queryPrefix, sizeof queryPrefix, "%s-query-%s", prefix, sel);

            for (size_t i = 0; i < q->stepItemList->itemCount; ++i)
                _genStepItem(f, queryPrefix, q->stepItemList->items[i]);
            break;
        }

        case ANIMATE_CHILD_ITEM: {
            _out(f, 0, "/* animateChild: apply child animations to nested elements */");
            _out(f, 0, ".%s > * {", prefix);
            _out(f, 1, "animation: inherit;");
            _out(f, 0, "}");
            break;
        }


        case STAGGER_ITEM: {
            Stagger *stag = (Stagger *)step->item;
            if (!stag) break;

            const char *staggerDelay = stag->time ? stag->time : "100ms";

            char staggerPrefix[256];
            snprintf(staggerPrefix, sizeof staggerPrefix, "%s-stagger-%u", prefix, _uniqueId++);

            _out(f, 0, ".%s {", staggerPrefix);
            _out(f, 1, "animation-fill-mode: forwards;");
            _out(f, 0, "}");

            for (int i = 1; i <= 10; ++i) {
                _out(f, 0, ".%s:nth-child(%d) {", staggerPrefix, i);
                if (strstr(staggerDelay, "ms")) {
                    int delayMs = atoi(staggerDelay) * (i - 1);
                    _out(f, 1, "animation-delay: %dms;", delayMs);
                } else if (strstr(staggerDelay, "s")) {
                    float delayS = atof(staggerDelay) * (i - 1);
                    _out(f, 1, "animation-delay: %.2fs;", delayS);
                } else {
                    int delayMs = 100 * (i - 1);
                    _out(f, 1, "animation-delay: %dms;", delayMs);
                }
                _out(f, 0, "}");
            }

            // Process nested step items within the stagger
            if (stag->stepItemList) {
                for (size_t i = 0; i < stag->stepItemList->itemCount; ++i)
                    _genStepItem(f, staggerPrefix, stag->stepItemList->items[i]);
            }
            break;
        }

        default:
            logError(_logger, "Unknown StepItem type %d", step->type);
    }
}

static void _genTransitionCss(FILE *f, Trigger *t, Transition *tr)
{
    if (!tr || !tr->transitionRule || !tr->transitionBlock) return;

    int  bidir = 0;
    const char *from = NULL, *to = NULL;

    if (tr->transitionRule->ruleType == FROM_TO) {
        from  = tr->transitionRule->fromState;
        to    = tr->transitionRule->toState;
        bidir = (tr->transitionRule->direction == BIDIRECTIONAL);
    } else {
        if (tr->transitionRule->alias == ENTER)      { from = "void"; to = "*";   }
        else if (tr->transitionRule->alias == LEAVE) { from = "*";    to = "void"; }
        else {
            const char *aliasStr = _aliasToState(tr->transitionRule->alias);
            from = to = aliasStr;
        }
    }

    // Avoid duplicating output for transitions like * <=> *
    if (bidir && from && to && strcmp(from, to) == 0) {
        bidir = 0;
    }

    // Get initial and final styles for this transition
    Style *fromStyle = NULL;
    Style *toStyle = NULL;

    // Look for explicit styles in the transition steps
    if (tr->transitionBlock && tr->transitionBlock->stepItemList) {
        StepItemList *sil = tr->transitionBlock->stepItemList;

        // First look for a style at the beginning of the transition block
        for (size_t i = 0; i < sil->itemCount; ++i) {
            StepItem *si = sil->items[i];
            if (!si) continue;

            if (si->type == STYLE_ITEM) {
                fromStyle = (Style *)si->item;
                break;
            } else if (si->type == GROUP_ITEM) {
                // For groups, look for style items inside the group
                Group *group = (Group *)si->item;
                if (group && group->stepItemList) {
                    for (size_t j = 0; j < group->stepItemList->itemCount; ++j) {
                        StepItem *groupItem = group->stepItemList->items[j];
                        if (groupItem && groupItem->type == STYLE_ITEM) {
                            fromStyle = (Style *)groupItem->item;
                            break;
                        }
                    }
                }
                if (fromStyle) break;
            }
        }

        // Then look for styles inside animate items
        for (size_t i = 0; i < sil->itemCount; ++i) {
            StepItem *si = sil->items[i];
            if (!si) continue;

            if (si->type == ANIMATE_ITEM) {
                Animate *animate = (Animate *)si->item;
                if (animate && animate->type == ANIMATE_WITH_STYLE) {
                    toStyle = animate->style;
                    break;
                }
            } else if (si->type == GROUP_ITEM) {
                // For groups, check for animate items with styles
                Group *group = (Group *)si->item;
                if (group && group->stepItemList) {
                    for (size_t j = 0; j < group->stepItemList->itemCount; ++j) {
                        StepItem *groupItem = group->stepItemList->items[j];
                        if (!groupItem || groupItem->type != ANIMATE_ITEM) continue;

                        Animate *animate = (Animate *)groupItem->item;
                        if (animate && animate->type == ANIMATE_WITH_STYLE) {
                            toStyle = animate->style;
                            break;
                        }
                    }
                }
                if (toStyle) break;
            }
        }
    }

    // Animation properties for the transition
    char *duration = "200ms";
    char *easing = "ease";
    char *delay = NULL;

    // Extract animation properties from any animate items
    StepItem *animateItem = _findFirstAnimateItem(tr->transitionBlock->stepItemList);
    if (animateItem && animateItem->type == ANIMATE_ITEM) {
        Animate *animate = (Animate *)animateItem->item;
        _extractAnimationProps(animate, &duration, &easing, &delay);
    }

    for (int pass = 0; pass < (bidir ? 2 : 1); ++pass) {
        const char *o = (pass == 0 ? from : to);
        const char *d = (pass == 0 ? to   : from);

        char prefix[128];
        int isEnterOrLeave = 0;
        int isSimpleStateTransition = 0;

        if (o && d) {  // Ensure we have valid state names
            if (strcmp(o, "void") == 0 && strcmp(d, "*") == 0) {
                snprintf(prefix, sizeof prefix, "%s-enter", t->name);
                isEnterOrLeave = 1;
            } else if (strcmp(o, "*") == 0 && strcmp(d, "void") == 0) {
                snprintf(prefix, sizeof prefix, "%s-leave", t->name);
                isEnterOrLeave = 1;
            } else if (strcmp(o, "void") != 0 && strcmp(d, "void") != 0 &&
                      strcmp(o, "*") != 0 && strcmp(d, "*") != 0) {
                // This is a regular state-to-state transition (not involving void or *)
                isSimpleStateTransition = 1;
                snprintf(prefix, sizeof prefix, "%s-%s-to-%s", t->name, _sanitize(o), _sanitize(d));
            } else {
                snprintf(prefix, sizeof prefix, "%s-%s-to-%s", t->name, _sanitize(o), _sanitize(d));
            }

            // Generate keyframe animations for each step
            if (tr->transitionBlock && tr->transitionBlock->stepItemList) {
                StepItemList *sil = tr->transitionBlock->stepItemList;
                int isSimpleAnimate = 0;

                // Process animations in this transition block
                if (sil) {
                    // If there's just one item and it's a simple animate (no style/keyframes)
                    if (sil->itemCount == 1 && sil->items[0] && sil->items[0]->type == ANIMATE_ITEM) {
                        Animate *animate = (Animate *)sil->items[0]->item;
                        if (animate->type == NONE) {
                            isSimpleAnimate = 1;
                        }
                    }
                }

                // Skip generating keyframes for simple state transitions with only basic animate
                if (isSimpleAnimate) {
                    continue; // Skip to the next iteration
                }
            }

            // Generate special CSS for enter/leave animations
            if (isEnterOrLeave) {
                // Check if this is an enter transition (void => *)
                if (strcmp(o, "void") == 0 && strcmp(d, "*") == 0) {
                    // Generate the entering class (for the animation while element is entering)
                    _out(f, 0, "/* Entering (void => *) */");
                    _out(f, 0, ".%s.entering {", t->name);

                    // Apply from style if provided
                    if (fromStyle && fromStyle->properties) {
                        _genStylePropsCss(f, 1, fromStyle->properties);
                    }

                    // Generate transition property with correct properties
                    if (fromStyle && fromStyle->properties) {
                        char transProps[512] = {0};
                        int hasTransProp = 0;

                        // Extract properties from the style to create a transition list
                        PropertyList *plist = fromStyle->properties;
                        for (size_t k = 0; k < plist->propertyCount; ++k) {
                            Property *p = plist->properties[k];
                            if (!p || !p->name) continue;

                            // Skip transform properties as they need special handling
                            if (strcmp(p->name, "scale") == 0 ||
                                strcmp(p->name, "translateX") == 0 ||
                                strcmp(p->name, "translateY") == 0 ||
                                strcmp(p->name, "rotate") == 0) {
                                continue;
                            }

                            if (hasTransProp) {
                                strncat(transProps, ", ", sizeof(transProps) - strlen(transProps) - 1);
                            }
                            strncat(transProps, p->name, sizeof(transProps) - strlen(transProps) - 1);
                            strncat(transProps, " ", sizeof(transProps) - strlen(transProps) - 1);
                            strncat(transProps, duration, sizeof(transProps) - strlen(transProps) - 1);
                            strncat(transProps, " ", sizeof(transProps) - strlen(transProps) - 1);
                            strncat(transProps, easing, sizeof(transProps) - strlen(transProps) - 1);
                            hasTransProp = 1;
                        }

                        // Add transform property if needed
                        int hasTransformProp = 0;
                        for (size_t k = 0; k < plist->propertyCount; ++k) {
                            Property *p = plist->properties[k];
                            if (!p || !p->name) continue;

                            if (strcmp(p->name, "scale") == 0 ||
                                strcmp(p->name, "translateX") == 0 ||
                                strcmp(p->name, "translateY") == 0 ||
                                strcmp(p->name, "rotate") == 0) {
                                hasTransformProp = 1;
                                break;
                            }
                        }

                        if (hasTransProp || hasTransformProp) {
                            _out(f, 1, "transition: %s;", transProps);
                        }
                    }

                    _out(f, 0, "}");

                    // Generate the entered class (final state after animation)
                    _out(f, 0, "/* Entered */");
                    _out(f, 0, ".%s.entered {", t->name);

                    // For entered state, we use default styles (opacity: 1, etc.)
                    // or the toStyle if provided
                    if (toStyle && toStyle->properties) {
                        _genStylePropsCss(f, 1, toStyle->properties);
                    } else {
                        // Default "entered" state has full opacity
                        _out(f, 1, "opacity: 1;");

                        // Copy other non-opacity properties from fromStyle
                        if (fromStyle && fromStyle->properties) {
                            PropertyList *plist = fromStyle->properties;
                            for (size_t k = 0; k < plist->propertyCount; ++k) {
                                Property *p = plist->properties[k];
                                if (!p || !p->name || strcmp(p->name, "opacity") == 0) continue;

                                // Copy the property value
                                char val[64] = {0};
                                switch (p->type) {
                                    case UNITLESS:
                                        snprintf(val, sizeof val, "%g", p->floatValue);
                                        break;
                                    case UNIT:
                                    case STRING:
                                    case COLOR:
                                        if (p->value && *p->value)
                                            strncpy(val, p->value, sizeof val - 1);
                                        else
                                            snprintf(val, sizeof val, "%g", p->floatValue);
                                        break;
                                    default:
                                        break;
                                }

                                _out(f, 1, "%s: %s;", p->name, val);
                            }
                        }
                    }

                    _out(f, 0, "}");
                }
                // Check if this is a leave transition (* => void)
                else if (strcmp(o, "*") == 0 && strcmp(d, "void") == 0) {
                    // Generate the exiting class
                    _out(f, 0, "/* Exiting (* => void) */");
                    _out(f, 0, ".%s.exiting {", t->name);

                    // First check if there's an explicit style at the beginning of transition block
                    // which would be the "from" style for the exit animation
                    Style *exitFromStyle = NULL;
                    if (tr->transitionBlock && tr->transitionBlock->stepItemList) {
                        StepItemList *sil = tr->transitionBlock->stepItemList;
                        for (size_t i = 0; i < sil->itemCount; ++i) {
                            StepItem *si = sil->items[i];
                            if (!si) continue;

                            if (si->type == STYLE_ITEM) {
                                exitFromStyle = (Style *)si->item;
                                break;
                            }
                        }
                    }

                    // If we have a style for the exit transition, apply it
                    if (exitFromStyle && exitFromStyle->properties) {
                        _genStylePropsCss(f, 1, exitFromStyle->properties);

                        // Generate transition property based on exitFromStyle properties
                        char transProps[512] = {0};
                        int hasTransProp = 0;

                        PropertyList *plist = exitFromStyle->properties;
                        for (size_t k = 0; k < plist->propertyCount; ++k) {
                            Property *p = plist->properties[k];
                            if (!p || !p->name) continue;

                            // Skip transform properties as they need special handling
                            if (strcmp(p->name, "scale") == 0 ||
                                strcmp(p->name, "translateX") == 0 ||
                                strcmp(p->name, "translateY") == 0 ||
                                strcmp(p->name, "rotate") == 0) {
                                continue;
                            }

                            if (hasTransProp) {
                                strncat(transProps, ", ", sizeof(transProps) - strlen(transProps) - 1);
                            }
                            strncat(transProps, p->name, sizeof(transProps) - strlen(transProps) - 1);
                            strncat(transProps, " ", sizeof(transProps) - strlen(transProps) - 1);
                            strncat(transProps, duration, sizeof(transProps) - strlen(transProps) - 1);
                            strncat(transProps, " ", sizeof(transProps) - strlen(transProps) - 1);
                            strncat(transProps, easing, sizeof(transProps) - strlen(transProps) - 1);
                            hasTransProp = 1;
                        }

                        // Add transform property if needed
                        int hasTransformProp = 0;
                        for (size_t k = 0; k < plist->propertyCount; ++k) {
                            Property *p = plist->properties[k];
                            if (!p || !p->name) continue;

                            if (strcmp(p->name, "scale") == 0 ||
                                strcmp(p->name, "translateX") == 0 ||
                                strcmp(p->name, "translateY") == 0 ||
                                strcmp(p->name, "rotate") == 0) {
                                hasTransformProp = 1;
                                break;
                            }
                        }

                        if (hasTransProp || hasTransformProp) {
                            _out(f, 1, "transition: %s;", transProps);
                        }
                    }

                    _out(f, 0, "}");
                }
            } else {
                // Regular state-to-state transition, generate keyframes and animation class
                _out(f, 0, "/* Transition from %s to %s */", o, d);
                _out(f, 0, "@keyframes %s {", prefix);

                _out(f, 1, "from {");
                if (fromStyle) {
                    _genStylePropsCss(f, 2, fromStyle->properties);
                }
                _out(f, 1, "}");

                _out(f, 1, "to {");
                if (toStyle) {
                    _genStylePropsCss(f, 2, toStyle->properties);
                }
                _out(f, 1, "}");

                _out(f, 0, "}");

                _out(f, 0, ".%s {", prefix);
                _out(f, 1, "animation-name: %s;", prefix);
                _out(f, 1, "animation-duration: %s;", duration);
                _out(f, 1, "animation-timing-function: %s;", easing);
                if (delay) {
                    _out(f, 1, "animation-delay: %s;", delay);
                }
                _out(f, 0, "}");
            }
        }
    }
}

static void _genProgramCss(FILE *f, Program *p)
{
    if (!p || !p->triggerList) {
        logError(_logger, "Empty program (no triggers).");
        return;
    }

    _out(f, 0, "/* Generated CSS */");

    // Process all triggers in the program
    for (size_t i = 0; i < p->triggerList->triggerCount; ++i) {
        Trigger *t = p->triggerList->trigger[i];
        if (t) {
            _genTriggerCss(f, t);
        }
    }
}

static StepItem *_findFirstAnimateItem(StepItemList *list)
{
    if (!list || !list->items)
        return NULL;

    // First pass: check for direct ANIMATE_ITEM
    for (size_t i = 0; i < list->itemCount; ++i) {
        StepItem *item = list->items[i];
        if (item && item->type == ANIMATE_ITEM)
            return item;
    }

    // Second pass: check for ANIMATE_ITEM inside GROUP or SEQUENCE
    for (size_t i = 0; i < list->itemCount; ++i) {
        StepItem *item = list->items[i];
        if (!item) continue;

        if (item->type == GROUP_ITEM && item->item) {
            Group *group = (Group *)item->item;
            if (group->stepItemList) {
                StepItem *found = _findFirstAnimateItem(group->stepItemList);
                if (found) return found;
            }
        }
        else if (item->type == SEQUENCE_ITEM && item->item) {
            Sequence *sequence = (Sequence *)item->item;
            if (sequence->stepItemList) {
                StepItem *found = _findFirstAnimateItem(sequence->stepItemList);
                if (found) return found;
            }
        }
    }

    return NULL;
}

static void _extractAnimationProps(Animate *a, char **propDuration, char **propEasing, char **propDelay)
{
    static char defaultDuration[] = "200ms";
    static char defaultEasing[] = "ease";
    static char defaultDelay[] = "0ms";

    if (!a || !a->animateInfo) {
        *propDuration = defaultDuration;
        *propEasing = defaultEasing;
        *propDelay = defaultDelay;
        return;
    }

    AnimateInfo *info = a->animateInfo;

    switch (info->type) {
        case DURATION:
            *propDuration = info->duration ? info->duration : defaultDuration;
            *propEasing = defaultEasing;
            *propDelay = defaultDelay;
            break;

        case DURATION_EASING:
            *propDuration = info->duration ? info->duration : defaultDuration;
            *propEasing = info->easing ? info->easing : defaultEasing;
            *propDelay = defaultDelay;
            break;

        case DURATION_DELAY:
            *propDuration = info->duration ? info->duration : defaultDuration;
            *propEasing = defaultEasing;
            *propDelay = info->delay ? info->delay : defaultDelay;
            break;

        case DURATION_DELAY_EASING:
            *propDuration = info->duration ? info->duration : defaultDuration;
            *propEasing = info->easing ? info->easing : defaultEasing;
            *propDelay = info->delay ? info->delay : defaultDelay;
            break;

        default:
            *propDuration = defaultDuration;
            *propEasing = defaultEasing;
            *propDelay = defaultDelay;
            break;
    }
}

static void _genTriggerCss(FILE *f, Trigger *t)
{
    if (!t) return;

    _out(f, 0, "/* Trigger: %s */", t->name);

    // Process states in the trigger block
    if (t->block && t->block->stateList) {
        StateList *sl = t->block->stateList;
        for (size_t i = 0; i < sl->stateCount; ++i) {
            State *s = sl->states[i];
            if (s) {
                _genStateClassCss(f, t->name, s, t->block);
            }
        }
    }

    // Process transitions in the trigger block
    if (t->block && t->block->transitionList) {
        TransitionList *tl = t->block->transitionList;
        for (size_t i = 0; i < tl->transitionCount; ++i) {
            Transition *tr = tl->transitions[i];
            if (tr) {
                _genTransitionCss(f, t, tr);
            }
        }
    }
}
