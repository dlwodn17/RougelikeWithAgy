#pragma once

#include "Core/Types.hpp"

class ElementalSystem {
public:
    // Core Reaction Resolution: Tests incoming element against target's active status bitmask
    static ReactionResult ResolveReaction(Element currentStatusMask, Element incomingElement, float weatherModifier = 1.0f);

    // Combination pair evaluation
    static ReactionResult EvaluateReactionPair(Element baseElement, Element incomingElement, float weatherModifier = 1.0f);

    // Spread elements for Gale / Wind reactions
    static std::vector<Element> GetActiveElementsFromMask(Element mask);

    // Utility text and color helpers
    static const char* GetElementName(Element elem);
    static const char* GetElementIcon(Element elem);
    static const char* GetElementShortDesc(Element elem);
    static ColorRGBA GetElementColor(Element elem);
};
