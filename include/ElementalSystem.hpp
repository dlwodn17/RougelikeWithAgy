#pragma once

#include "Common.hpp"

struct ReactionOutcome {
    bool triggered = false;
    std::string reactionName;
    std::string description;
    Element consumedElement = Element::NONE;
    int bonusDamage = 0;
    bool freezeTarget = false;
    bool chainAoE = false;
    int aoeDamage = 0;
    bool applyBurn = false;
    bool applyOil = false;
    bool cleanseDebuffs = false;
    Color reactionColor = WHITE;
};

class ElementalSystem {
public:
    static const char* GetElementName(Element elem);
    static const char* GetElementIcon(Element elem);
    static std::string GetElementDescription(Element elem);
    static Color GetElementColor(Element elem);

    // Apply or refresh an element on an entity's status buffer
    static void ApplyElement(std::vector<StatusInstance>& buffer, Element element, int duration = 2, int stacks = 1);

    // Check if buffer contains a specific element
    static bool HasElement(const std::vector<StatusInstance>& buffer, Element element);

    // Remove element from buffer
    static bool RemoveElement(std::vector<StatusInstance>& buffer, Element element);

    // Process elemental reactions when a new element interacts with existing buffer
    static ReactionOutcome ProcessReaction(std::vector<StatusInstance>& buffer, Element incomingElement, int baseDamage);

    // Spread elements from a source buffer to destination buffers (Gale wind mechanic)
    static std::vector<Element> GetSpreadElements(const std::vector<StatusInstance>& sourceBuffer);
};
