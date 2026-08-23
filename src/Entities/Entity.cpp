#include "Entities/Entity.hpp"
#include <cstdlib>

Entity::Entity(const std::string& entityName, int maxHealth)
    : name(entityName), hp(maxHealth), maxHp(maxHealth), shield(0),
      stance(StanceType::ATTACK), activeStatusMask(Element::NONE),
      isFrozen(false), posX(0.0f), posY(0.0f), hitShakeTimer(0.0f), flashTimer(0.0f) {
}

DamageReport Entity::ApplyIncomingDamage(int incomingDamage, Element incomingElement, StanceType targetStance, float weatherModifier) {
    DamageReport report;
    report.rawDamage = incomingDamage;

    // 1. Evaluate Stance Mitigation
    float stanceMult = 1.0f;
    if (targetStance == StanceType::DEFENSE) {
        stanceMult = 0.70f; // -30% Damage
    } else if (targetStance == StanceType::PARRY) {
        stanceMult = 0.50f; // -50% Damage & Parry flag
        report.wasParried = true;
    }

    int effectiveDamage = static_cast<int>(incomingDamage * stanceMult);

    // 2. Evaluate Reaction with Target's Active Status Mask
    if (incomingElement != Element::NONE && activeStatusMask != Element::NONE) {
        ReactionResult reaction = ElementalSystem::ResolveReaction(activeStatusMask, incomingElement, weatherModifier);
        if (reaction.triggered) {
            report.reaction = reaction;
            effectiveDamage = static_cast<int>(effectiveDamage * reaction.damageMultiplier) + reaction.bonusDamage;

            // Clear consumed elements from mask
            ClearElement(reaction.consumedElements);

            // Apply newly generated reaction elements (e.g. Fire from Explosion)
            if (reaction.appliedElements != Element::NONE) {
                ApplyElement(reaction.appliedElements, reaction.appliedDuration);
            }

            if (reaction.stunTarget) {
                isFrozen = true;
            }
        }
    }

    // If no reaction occurred or consumed the element, apply incoming element to mask
    if (!report.reaction.triggered && incomingElement != Element::NONE) {
        ApplyElement(incomingElement, 2);
    }

    // 3. Shield Absorption
    report.mitigatedDamage = effectiveDamage;
    int damageRemaining = effectiveDamage;

    if (shield > 0) {
        report.wasShielded = true;
        if (shield >= damageRemaining) {
            report.shieldAbsorbed = damageRemaining;
            shield -= damageRemaining;
            damageRemaining = 0;
        } else {
            report.shieldAbsorbed = shield;
            damageRemaining -= shield;
            shield = 0;
        }
    }

    // 4. HP Reduction
    report.healthDamage = damageRemaining;
    hp -= damageRemaining;
    if (hp <= 0) {
        hp = 0;
        report.causedDeath = true;
    }

    // Visual timers
    hitShakeTimer = 0.25f;
    flashTimer = 0.2f;

    return report;
}

void Entity::ApplyElement(Element elem, int duration) {
    if (elem == Element::NONE) return;

    static const Element elements[] = {
        Element::WET, Element::FIRE, Element::OIL, Element::LIGHTNING, Element::COLD, Element::GALE
    };

    for (Element e : elements) {
        if (HasFlag(elem, e)) {
            activeStatusMask |= e;
            uint32_t key = static_cast<uint32_t>(e);
            if (statusDurations.find(key) != statusDurations.end()) {
                statusDurations[key] = std::max(statusDurations[key], duration);
            } else {
                statusDurations[key] = duration;
            }
        }
    }
}

void Entity::TickStatusEffects(std::vector<std::string>& outLogs) {
    static const Element elements[] = {
        Element::WET, Element::FIRE, Element::OIL, Element::LIGHTNING, Element::COLD, Element::GALE
    };

    for (Element e : elements) {
        if (HasFlag(activeStatusMask, e)) {
            uint32_t key = static_cast<uint32_t>(e);

            // Fire DoT damage
            if (e == Element::FIRE) {
                int dot = 6;
                hp -= dot;
                if (hp < 0) hp = 0;
                hitShakeTimer = 0.15f;
                outLogs.push_back(name + " suffered " + std::to_string(dot) + " Burning Fire DoT damage!");
            }

            statusDurations[key]--;
            if (statusDurations[key] <= 0) {
                outLogs.push_back(name + "'s [" + ElementalSystem::GetElementName(e) + "] effect expired.");
                ClearElement(e);
            }
        }
    }
}

void Entity::ClearElement(Element elem) {
    if (elem == Element::NONE) return;

    static const Element elements[] = {
        Element::WET, Element::FIRE, Element::OIL, Element::LIGHTNING, Element::COLD, Element::GALE
    };

    for (Element e : elements) {
        if (HasFlag(elem, e)) {
            activeStatusMask = static_cast<Element>(static_cast<uint32_t>(activeStatusMask) & ~static_cast<uint32_t>(e));
            statusDurations.erase(static_cast<uint32_t>(e));
        }
    }
}

void Entity::ClearAllElements() {
    activeStatusMask = Element::NONE;
    statusDurations.clear();
}

bool Entity::HasElement(Element elem) const {
    if (elem == Element::NONE) return false;
    return HasFlag(activeStatusMask, elem);
}

int Entity::GetElementDuration(Element elem) const {
    uint32_t key = static_cast<uint32_t>(elem);
    auto it = statusDurations.find(key);
    if (it != statusDurations.end()) {
        return it->second;
    }
    return 0;
}

std::vector<StatusInstance> Entity::GetStatusInstances() const {
    std::vector<StatusInstance> instances;
    static const Element elements[] = {
        Element::WET, Element::FIRE, Element::OIL, Element::LIGHTNING, Element::COLD, Element::GALE
    };
    for (Element e : elements) {
        if (HasFlag(activeStatusMask, e)) {
            instances.emplace_back(e, GetElementDuration(e), 1);
        }
    }
    return instances;
}

void Entity::Heal(int amount) {
    hp += amount;
    if (hp > maxHp) hp = maxHp;
}

void Entity::UpdateVisuals(float dt) {
    if (hitShakeTimer > 0.0f) hitShakeTimer -= dt;
    if (flashTimer > 0.0f) flashTimer -= dt;
}

Vec2 Entity::GetRenderOffset() const {
    if (hitShakeTimer > 0.0f) {
        float shakeMag = (hitShakeTimer / 0.25f) * 6.0f;
        float offsetX = ((float)rand() / (float)RAND_MAX * 2.0f - 1.0f) * shakeMag;
        float offsetY = ((float)rand() / (float)RAND_MAX * 2.0f - 1.0f) * shakeMag;
        return Vec2{ offsetX, offsetY };
    }
    return Vec2{ 0.0f, 0.0f };
}

void Entity::GetRenderOffset(float& outX, float& outY) const {
    if (hitShakeTimer > 0.0f) {
        float shakeMag = (hitShakeTimer / 0.25f) * 6.0f;
        outX = ((float)rand() / (float)RAND_MAX * 2.0f - 1.0f) * shakeMag;
        outY = ((float)rand() / (float)RAND_MAX * 2.0f - 1.0f) * shakeMag;
    } else {
        outX = 0.0f;
        outY = 0.0f;
    }
}

