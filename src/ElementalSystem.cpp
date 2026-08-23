#include "ElementalSystem.hpp"

const char* ElementalSystem::GetElementName(Element elem) {
    switch (elem) {
        case Element::WET:       return "Wet";
        case Element::FIRE:      return "Fire";
        case Element::LIGHTNING: return "Lightning";
        case Element::COLD:      return "Cold";
        case Element::OIL:       return "Oil";
        case Element::GALE:      return "Gale";
        case Element::NONE:
        default:                 return "None";
    }
}

const char* ElementalSystem::GetElementIcon(Element elem) {
    switch (elem) {
        case Element::WET:       return "💧 WET";
        case Element::FIRE:      return "🔥 FIRE";
        case Element::LIGHTNING: return "⚡ ELEC";
        case Element::COLD:      return "❄️ COLD";
        case Element::OIL:       return "🛢️ OIL";
        case Element::GALE:      return "🌪️ GALE";
        case Element::NONE:
        default:                 return "• NONE";
    }
}

std::string ElementalSystem::GetElementDescription(Element elem) {
    switch (elem) {
        case Element::WET:
            return "Drenched. Reacts with Lightning (Shock) or Cold (Frozen). Extinguishes Fire.";
        case Element::FIRE:
            return "Burning heat. Reacts with Oil (Explosion) or Cold (Melt). Deals DoT on turn tick.";
        case Element::LIGHTNING:
            return "Electrified charge. Reacts with Wet (Shock AoE chain) or Oil (Plasma).";
        case Element::COLD:
            return "Chilled frost. Reacts with Wet (Freezes target, skipping next turn).";
        case Element::OIL:
            return "Highly flammable sludge. Reacts with Fire (Massive explosion + burning ground).";
        case Element::GALE:
            return "Swirling winds. Spreads active status effects to neighboring entities.";
        case Element::NONE:
        default:
            return "No active elemental properties.";
    }
}

Color ElementalSystem::GetElementColor(Element elem) {
    return GetElementBaseColor(elem);
}

void ElementalSystem::ApplyElement(std::vector<StatusInstance>& buffer, Element element, int duration, int stacks) {
    if (element == Element::NONE) return;

    for (auto& status : buffer) {
        if (status.element == element) {
            status.duration = std::max(status.duration, duration);
            status.stacks = std::min(status.stacks + stacks, 5);
            return;
        }
    }

    buffer.emplace_back(element, duration, stacks);
}

bool ElementalSystem::HasElement(const std::vector<StatusInstance>& buffer, Element element) {
    for (const auto& status : buffer) {
        if (status.element == element && status.duration > 0) return true;
    }
    return false;
}

bool ElementalSystem::RemoveElement(std::vector<StatusInstance>& buffer, Element element) {
    for (auto it = buffer.begin(); it != buffer.end(); ++it) {
        if (it->element == element) {
            buffer.erase(it);
            return true;
        }
    }
    return false;
}

ReactionOutcome ElementalSystem::ProcessReaction(std::vector<StatusInstance>& buffer, Element incomingElement, int baseDamage) {
    ReactionOutcome outcome;
    if (incomingElement == Element::NONE) return outcome;

    // 1. WET + LIGHTNING = SHOCK (Chain AoE damage + Burst)
    if ((incomingElement == Element::LIGHTNING && HasElement(buffer, Element::WET)) ||
        (incomingElement == Element::WET && HasElement(buffer, Element::LIGHTNING))) {
        
        outcome.triggered = true;
        outcome.reactionName = "SHOCK";
        outcome.description = "Electrified shockwave arcs through the drenched target!";
        outcome.consumedElement = (incomingElement == Element::LIGHTNING) ? Element::WET : Element::LIGHTNING;
        outcome.bonusDamage = static_cast<int>(baseDamage * 0.6f) + 12;
        outcome.chainAoE = true;
        outcome.aoeDamage = static_cast<int>(baseDamage * 0.45f) + 8;
        outcome.reactionColor = (Color){ 241, 196, 15, 255 };

        RemoveElement(buffer, outcome.consumedElement);
        return outcome;
    }

    // 2. OIL + FIRE = EXPLOSION (Bonus burst + burning)
    if ((incomingElement == Element::FIRE && HasElement(buffer, Element::OIL)) ||
        (incomingElement == Element::OIL && HasElement(buffer, Element::FIRE))) {
        
        outcome.triggered = true;
        outcome.reactionName = "EXPLOSION";
        outcome.description = "Oil ignites into a cataclysmic blast!";
        outcome.consumedElement = (incomingElement == Element::FIRE) ? Element::OIL : Element::FIRE;
        outcome.bonusDamage = static_cast<int>(baseDamage * 1.0f) + 20;
        outcome.applyBurn = true;
        outcome.reactionColor = (Color){ 231, 76, 60, 255 };

        RemoveElement(buffer, outcome.consumedElement);
        // Apply burning status after explosion
        ApplyElement(buffer, Element::FIRE, 3, 2);
        return outcome;
    }

    // 3. WET + COLD = FROZEN (Target is frozen, skips next action)
    if ((incomingElement == Element::COLD && HasElement(buffer, Element::WET)) ||
        (incomingElement == Element::WET && HasElement(buffer, Element::COLD))) {
        
        outcome.triggered = true;
        outcome.reactionName = "FROZEN";
        outcome.description = "Moisture flash freezes into solid ice! Target immobilized.";
        outcome.consumedElement = (incomingElement == Element::COLD) ? Element::WET : Element::COLD;
        outcome.bonusDamage = static_cast<int>(baseDamage * 0.35f) + 6;
        outcome.freezeTarget = true;
        outcome.reactionColor = (Color){ 162, 222, 255, 255 };

        RemoveElement(buffer, outcome.consumedElement);
        return outcome;
    }

    // 4. FIRE + COLD = MELT / STEAM (Vaporize burst damage)
    if ((incomingElement == Element::FIRE && HasElement(buffer, Element::COLD)) ||
        (incomingElement == Element::COLD && HasElement(buffer, Element::FIRE))) {
        
        outcome.triggered = true;
        outcome.reactionName = "MELT";
        outcome.description = "Rapid thermodynamic collapse releases superheated steam!";
        outcome.consumedElement = (incomingElement == Element::FIRE) ? Element::COLD : Element::FIRE;
        outcome.bonusDamage = static_cast<int>(baseDamage * 0.75f) + 10;
        outcome.cleanseDebuffs = false;
        outcome.reactionColor = (Color){ 243, 156, 18, 255 };

        RemoveElement(buffer, outcome.consumedElement);
        return outcome;
    }

    // 5. OIL + LIGHTNING = PLASMA
    if ((incomingElement == Element::LIGHTNING && HasElement(buffer, Element::OIL)) ||
        (incomingElement == Element::OIL && HasElement(buffer, Element::LIGHTNING))) {
        
        outcome.triggered = true;
        outcome.reactionName = "PLASMA";
        outcome.description = "High voltage arcs through combustible hydrocarbon!";
        outcome.consumedElement = (incomingElement == Element::LIGHTNING) ? Element::OIL : Element::LIGHTNING;
        outcome.bonusDamage = static_cast<int>(baseDamage * 0.85f) + 15;
        outcome.reactionColor = (Color){ 155, 89, 182, 255 };

        RemoveElement(buffer, outcome.consumedElement);
        return outcome;
    }

    // If no reaction occurred, simply add/refresh the incoming element on buffer
    ApplyElement(buffer, incomingElement, 2, 1);
    return outcome;
}

std::vector<Element> ElementalSystem::GetSpreadElements(const std::vector<StatusInstance>& sourceBuffer) {
    std::vector<Element> spreadList;
    for (const auto& status : sourceBuffer) {
        if (status.element != Element::NONE) {
            spreadList.push_back(status.element);
        }
    }
    return spreadList;
}
