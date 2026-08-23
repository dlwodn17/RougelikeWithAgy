#include "Systems/ElementalSystem.hpp"

ReactionOutcome ElementalSystem::EvaluateReaction(Element baseElement, Element incomingElement, float weatherModifier) {
    ReactionOutcome outcome;
    outcome.triggered = false;

    if (baseElement == Element::NONE || incomingElement == Element::NONE) {
        return outcome;
    }

    // Reaction 1: WET + LIGHTNING -> SHOCK (Chain AoE burst)
    if ((baseElement == Element::WET && incomingElement == Element::LIGHTNING) ||
        (baseElement == Element::LIGHTNING && incomingElement == Element::WET)) {
        outcome.triggered = true;
        outcome.reactionName = "SHOCK";
        outcome.description = "Electrified water conducts high-voltage arcs across all enemies!";
        outcome.reactionColor = (Color){ 241, 196, 15, 255 }; // Electric Gold
        outcome.bonusDamage = static_cast<int>(18 * weatherModifier);
        outcome.chainAoE = true;
        outcome.aoeDamage = static_cast<int>(12 * weatherModifier);
        return outcome;
    }

    // Reaction 2: OIL + FIRE -> EXPLOSION (Cataclysmic burst + Burn DoT)
    if ((baseElement == Element::OIL && incomingElement == Element::FIRE) ||
        (baseElement == Element::FIRE && incomingElement == Element::OIL)) {
        outcome.triggered = true;
        outcome.reactionName = "EXPLOSION";
        outcome.description = "Volatile chemical reaction detonates in a massive burst!";
        outcome.reactionColor = (Color){ 231, 76, 60, 255 }; // Fire Red
        outcome.bonusDamage = static_cast<int>(22 * weatherModifier);
        outcome.applyBurn = true;
        outcome.burnDuration = 3;
        return outcome;
    }

    // Reaction 3: WET + COLD -> FROZEN (Stun target for 1 turn)
    if ((baseElement == Element::WET && incomingElement == Element::COLD) ||
        (baseElement == Element::COLD && incomingElement == Element::WET)) {
        outcome.triggered = true;
        outcome.reactionName = "FROZEN";
        outcome.description = "Target is encased in solid ice and skips their next action!";
        outcome.reactionColor = (Color){ 162, 222, 255, 255 }; // Ice Cyan
        outcome.bonusDamage = static_cast<int>(10 * weatherModifier);
        outcome.stunTarget = true;
        return outcome;
    }

    // Reaction 4: FIRE + COLD -> MELT / STEAM (Vaporization bonus damage)
    if ((baseElement == Element::FIRE && incomingElement == Element::COLD) ||
        (baseElement == Element::COLD && incomingElement == Element::FIRE)) {
        outcome.triggered = true;
        outcome.reactionName = "MELT";
        outcome.description = "Superheated steam blast vaporizes defenses!";
        outcome.reactionColor = (Color){ 243, 156, 18, 255 }; // Amber Orange
        outcome.bonusDamage = static_cast<int>(15 * weatherModifier);
        return outcome;
    }

    // Reaction 5: OIL + LIGHTNING -> PLASMA (Piercing electric strike)
    if ((baseElement == Element::OIL && incomingElement == Element::LIGHTNING) ||
        (baseElement == Element::LIGHTNING && incomingElement == Element::OIL)) {
        outcome.triggered = true;
        outcome.reactionName = "PLASMA";
        outcome.description = "High-energy plasma discharge pierces defenses!";
        outcome.reactionColor = (Color){ 155, 89, 182, 255 }; // Arcane Purple
        outcome.bonusDamage = static_cast<int>(16 * weatherModifier);
        return outcome;
    }

    return outcome;
}

std::vector<Element> ElementalSystem::GetSpreadElements(const std::vector<StatusInstance>& currentStatuses) {
    std::vector<Element> spreadList;
    for (const auto& status : currentStatuses) {
        if (status.element != Element::NONE && status.duration > 0) {
            spreadList.push_back(status.element);
        }
    }
    return spreadList;
}

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
        default:                 return "⚪ NONE";
    }
}

const char* ElementalSystem::GetElementShortDesc(Element elem) {
    switch (elem) {
        case Element::WET:       return "Conducts electricity & freezes easily";
        case Element::FIRE:      return "Deals 6 DoT per turn & burns oil";
        case Element::LIGHTNING: return "Arcs across wet targets";
        case Element::COLD:      return "Slows down and solidifies wet targets";
        case Element::OIL:       return "Highly combustible fuel source";
        case Element::GALE:      return "Disperses statuses to adjacent targets";
        case Element::NONE:
        default:                 return "No effect";
    }
}

Color ElementalSystem::GetElementColor(Element elem) {
    return GetElementBaseColor(elem);
}
