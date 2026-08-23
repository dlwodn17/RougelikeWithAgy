#pragma once

#include "Core/Types.hpp"

class ElementalSystem {
public:
    static ReactionOutcome EvaluateReaction(Element baseElement, Element incomingElement, float weatherModifier = 1.0f);
    static std::vector<Element> GetSpreadElements(const std::vector<StatusInstance>& currentStatuses);

    static const char* GetElementName(Element elem);
    static const char* GetElementIcon(Element elem);
    static const char* GetElementShortDesc(Element elem);
    static Color GetElementColor(Element elem);
};
