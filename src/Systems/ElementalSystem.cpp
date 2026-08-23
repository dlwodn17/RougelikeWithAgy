#include "Systems/ElementalSystem.hpp"

ReactionResult ElementalSystem::EvaluateReactionPair(Element baseElement, Element incomingElement, float weatherModifier) {
    ReactionResult result;
    result.triggered = false;
    result.type = ReactionType::NONE;
    result.damageMultiplier = 1.0f;

    if (baseElement == Element::NONE || incomingElement == Element::NONE) {
        return result;
    }

    // Reaction 1: WET + LIGHTNING = SHOCK (Chain AoE + Burst)
    if ((baseElement == Element::WET && incomingElement == Element::LIGHTNING) ||
        (baseElement == Element::LIGHTNING && incomingElement == Element::WET)) {
        result.triggered = true;
        result.type = ReactionType::SHOCK;
        result.name = "SHOCK";
        result.description = "Electrified water conducts high-voltage arcs across all enemies!";
        result.bonusDamage = static_cast<int>(18 * weatherModifier);
        result.damageMultiplier = 1.25f;
        result.consumedElements = Element::WET | Element::LIGHTNING;
        result.chainAoE = true;
        result.aoeDamage = static_cast<int>(12 * weatherModifier);
        return result;
    }

    // Reaction 2: OIL + FIRE = EXPLOSION (Cataclysmic burst + Burning DoT)
    if ((baseElement == Element::OIL && incomingElement == Element::FIRE) ||
        (baseElement == Element::FIRE && incomingElement == Element::OIL)) {
        result.triggered = true;
        result.type = ReactionType::EXPLOSION;
        result.name = "EXPLOSION";
        result.description = "Volatile chemical reaction detonates in a massive burst and ignites target!";
        result.bonusDamage = static_cast<int>(22 * weatherModifier);
        result.damageMultiplier = 1.40f;
        result.consumedElements = Element::OIL;
        result.appliedElements = Element::FIRE;
        result.appliedDuration = 3;
        return result;
    }

    // Reaction 3: WET + COLD = FROZEN (1 Turn Immobilization)
    if ((baseElement == Element::WET && incomingElement == Element::COLD) ||
        (baseElement == Element::COLD && incomingElement == Element::WET)) {
        result.triggered = true;
        result.type = ReactionType::FROZEN;
        result.name = "FROZEN";
        result.description = "Target is encased in solid ice and skips their next action!";
        result.bonusDamage = static_cast<int>(10 * weatherModifier);
        result.damageMultiplier = 1.10f;
        result.consumedElements = Element::WET | Element::COLD;
        result.stunTarget = true;
        return result;
    }

    // Reaction 4: FIRE + COLD = MELT / STEAM (Vaporization Bonus Damage)
    if ((baseElement == Element::FIRE && incomingElement == Element::COLD) ||
        (baseElement == Element::COLD && incomingElement == Element::FIRE)) {
        result.triggered = true;
        result.type = ReactionType::MELT;
        result.name = "MELT";
        result.description = "Superheated steam blast vaporizes defenses!";
        result.bonusDamage = static_cast<int>(15 * weatherModifier);
        result.damageMultiplier = 1.30f;
        result.consumedElements = Element::FIRE | Element::COLD;
        return result;
    }

    // Reaction 5: OIL + LIGHTNING = PLASMA (Armor-Piercing Electric Discharge)
    if ((baseElement == Element::OIL && incomingElement == Element::LIGHTNING) ||
        (baseElement == Element::LIGHTNING && incomingElement == Element::OIL)) {
        result.triggered = true;
        result.type = ReactionType::PLASMA;
        result.name = "PLASMA";
        result.description = "High-energy plasma discharge pierces defenses!";
        result.bonusDamage = static_cast<int>(16 * weatherModifier);
        result.damageMultiplier = 1.20f;
        result.consumedElements = Element::OIL | Element::LIGHTNING;
        return result;
    }

    return result;
}

ReactionResult ElementalSystem::ResolveReaction(Element currentStatusMask, Element incomingElement, float weatherModifier) {
    ReactionResult finalResult;
    finalResult.triggered = false;
    finalResult.damageMultiplier = 1.0f;

    if (currentStatusMask == Element::NONE || incomingElement == Element::NONE) {
        return finalResult;
    }

    // Test each active bit in the mask against incomingElement
    static const Element testElements[] = {
        Element::WET, Element::FIRE, Element::OIL, Element::LIGHTNING, Element::COLD, Element::GALE
    };

    for (Element base : testElements) {
        if (HasFlag(currentStatusMask, base)) {
            ReactionResult res = EvaluateReactionPair(base, incomingElement, weatherModifier);
            if (res.triggered) {
                return res;
            }
        }
    }

    return finalResult;
}

std::vector<Element> ElementalSystem::GetActiveElementsFromMask(Element mask) {
    std::vector<Element> list;
    static const Element testElements[] = {
        Element::WET, Element::FIRE, Element::OIL, Element::LIGHTNING, Element::COLD, Element::GALE
    };
    for (Element elem : testElements) {
        if (HasFlag(mask, elem)) {
            list.push_back(elem);
        }
    }
    return list;
}

const char* ElementalSystem::GetElementName(Element elem) {
    return GetElementNameStr(elem);
}

const char* ElementalSystem::GetElementIcon(Element elem) {
    return GetElementSymbolStr(elem);
}

const char* ElementalSystem::GetElementShortDesc(Element elem) {
    switch (elem) {
        case Element::WET:       return "Conducts electricity (SHOCK) & freezes (FROZEN)";
        case Element::FIRE:      return "Deals 6 DoT per turn & ignites OIL (EXPLOSION)";
        case Element::LIGHTNING: return "Arcs across WET targets causing SHOCK AoE";
        case Element::COLD:      return "Solidifies WET targets into FROZEN stun";
        case Element::OIL:       return "Highly combustible fuel; triggers EXPLOSION on FIRE";
        case Element::GALE:      return "Disperses statuses to adjacent combatants";
        case Element::NONE:
        default:                 return "No effect";
    }
}

ColorRGBA ElementalSystem::GetElementColor(Element elem) {
    return GetElementColorRGBA(elem);
}
