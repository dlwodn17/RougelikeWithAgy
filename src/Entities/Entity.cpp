#include "Entities/Entity.hpp"

Entity::Entity(const std::string& entityName, int maxHealth)
    : name(entityName), hp(maxHealth), maxHp(maxHealth), shield(0), isFrozen(false),
      position((Vector2){ 0, 0 }), hitShakeTimer(0.0f), flashTimer(0.0f) {
}

DamageReport Entity::ApplyIncomingDamage(int incomingDamage, Element incomingElement, StanceType stance, float weatherModifier) {
    DamageReport report;
    report.rawDamage = incomingDamage;

    // 1. Evaluate Stance Damage Mitigation
    float stanceMult = 1.0f;
    if (stance == StanceType::DEFENSE) {
        stanceMult = 0.70f; // -30% Damage Taken
    } else if (stance == StanceType::PARRY) {
        stanceMult = 0.50f; // -50% Damage Taken
        report.wasParried = true;
    }

    int effectiveDamage = static_cast<int>(incomingDamage * stanceMult);

    // 2. Evaluate Reactions with active status buffer
    if (incomingElement != Element::NONE && !statusBuffer.empty()) {
        for (auto it = statusBuffer.begin(); it != statusBuffer.end(); ++it) {
            ReactionOutcome reaction = ElementalSystem::EvaluateReaction(it->element, incomingElement, weatherModifier);
            if (reaction.triggered) {
                report.reaction = reaction;
                effectiveDamage += reaction.bonusDamage;

                if (reaction.stunTarget) {
                    isFrozen = true;
                }
                if (reaction.applyBurn) {
                    InflictElement(Element::FIRE, reaction.burnDuration);
                }

                // Consume reacting status
                statusBuffer.erase(it);
                break;
            }
        }
    }

    // If no reaction consumed it, apply status to buffer
    if (!report.reaction.triggered && incomingElement != Element::NONE) {
        InflictElement(incomingElement, 2);
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

    // 4. Health Damage
    report.healthDamage = damageRemaining;
    hp -= damageRemaining;
    if (hp <= 0) {
        hp = 0;
        report.causedDeath = true;
    }

    // Visual feedback triggers
    hitShakeTimer = 0.25f;
    flashTimer = 0.2f;

    return report;
}

void Entity::InflictElement(Element elem, int duration) {
    if (elem == Element::NONE) return;

    for (auto& status : statusBuffer) {
        if (status.element == elem) {
            status.duration = std::max(status.duration, duration);
            status.stacks++;
            return;
        }
    }

    statusBuffer.emplace_back(elem, duration, 1);
}

void Entity::TickStatusEffects(std::vector<std::string>& outLogs) {
    for (auto it = statusBuffer.begin(); it != statusBuffer.end();) {
        // Fire DoT damage
        if (it->element == Element::FIRE) {
            int dot = 6 * it->stacks;
            hp -= dot;
            if (hp < 0) hp = 0;
            hitShakeTimer = 0.15f;
            outLogs.push_back(name + " suffered " + std::to_string(dot) + " Burning damage!");
        }

        it->duration--;
        if (it->duration <= 0) {
            outLogs.push_back(name + "'s [" + ElementalSystem::GetElementName(it->element) + "] status dissipated.");
            it = statusBuffer.erase(it);
        } else {
            ++it;
        }
    }
}

void Entity::ClearElement(Element elem) {
    for (auto it = statusBuffer.begin(); it != statusBuffer.end();) {
        if (it->element == elem) {
            it = statusBuffer.erase(it);
        } else {
            ++it;
        }
    }
}

bool Entity::HasElement(Element elem) const {
    for (const auto& status : statusBuffer) {
        if (status.element == elem && status.duration > 0) return true;
    }
    return false;
}

void Entity::Heal(int amount) {
    hp += amount;
    if (hp > maxHp) hp = maxHp;
}

void Entity::UpdateVisuals(float dt) {
    if (hitShakeTimer > 0.0f) hitShakeTimer -= dt;
    if (flashTimer > 0.0f) flashTimer -= dt;
}

Vector2 Entity::GetRenderOffset() const {
    if (hitShakeTimer > 0.0f) {
        float shakeMag = (hitShakeTimer / 0.25f) * 6.0f;
        float offsetX = ((float)rand() / (float)RAND_MAX * 2.0f - 1.0f) * shakeMag;
        float offsetY = ((float)rand() / (float)RAND_MAX * 2.0f - 1.0f) * shakeMag;
        return (Vector2){ offsetX, offsetY };
    }
    return (Vector2){ 0.0f, 0.0f };
}
