#include "Entity.hpp"
#include <random>

Entity::Entity(std::string name, int maxHp, Color color)
    : name(name), hp(maxHp), maxHp(maxHp), shield(0), isFrozen(false), isStunned(false),
      visualPos({0, 0}), targetPos({0, 0}), primaryColor(color), hitShakeTimer(0.0f), flashTimer(0.0f) {
}

void Entity::AddShield(int amount) {
    shield += amount;
}

void Entity::ResetShield() {
    shield = 0;
}

void Entity::Heal(int amount) {
    hp = std::min(maxHp, hp + amount);
}

void Entity::InflictElement(Element elem, int duration, int stacks) {
    ElementalSystem::ApplyElement(statusBuffer, elem, duration, stacks);
}

bool Entity::HasElement(Element elem) const {
    return ElementalSystem::HasElement(statusBuffer, elem);
}

void Entity::ClearElement(Element elem) {
    ElementalSystem::RemoveElement(statusBuffer, elem);
}

DamageReport Entity::ApplyIncomingDamage(int rawDamage, Element element, StanceType receiverStance, float weatherMultiplier) {
    DamageReport report;
    report.rawDamage = rawDamage;

    // Apply elemental reaction if an element is incoming
    if (element != Element::NONE) {
        report.reaction = ElementalSystem::ProcessReaction(statusBuffer, element, rawDamage);
    }

    int totalDamage = static_cast<int>(rawDamage * weatherMultiplier);
    if (report.reaction.triggered) {
        totalDamage += report.reaction.bonusDamage;
        if (report.reaction.freezeTarget) {
            isFrozen = true;
        }
    }

    // Stance mitigation
    if (receiverStance == StanceType::DEFENSE) {
        totalDamage = static_cast<int>(totalDamage * 0.70f); // 30% dmg reduction
        report.wasShielded = true;
    } else if (receiverStance == StanceType::PARRY) {
        totalDamage = static_cast<int>(totalDamage * 0.50f); // 50% dmg reduction
        report.wasParried = true;
    }

    report.mitigatedDamage = std::max(1, totalDamage);

    // Apply shield absorption
    if (shield > 0) {
        if (shield >= report.mitigatedDamage) {
            report.shieldAbsorbed = report.mitigatedDamage;
            shield -= report.mitigatedDamage;
            report.healthDamage = 0;
        } else {
            report.shieldAbsorbed = shield;
            report.healthDamage = report.mitigatedDamage - shield;
            shield = 0;
        }
    } else {
        report.healthDamage = report.mitigatedDamage;
    }

    hp = std::max(0, hp - report.healthDamage);
    if (hp == 0) {
        report.causedDeath = true;
    }

    TriggerHitVisuals();
    return report;
}

void Entity::TickStatusEffects(std::vector<std::string>& logs) {
    // 1. Tick Fire Burn DoT
    if (HasElement(Element::FIRE)) {
        int burnDmg = 6;
        hp = std::max(0, hp - burnDmg);
        logs.push_back(name + " takes " + std::to_string(burnDmg) + " Burn damage!");
        TriggerHitVisuals();
    }

    // 2. Decrement durations
    for (auto it = statusBuffer.begin(); it != statusBuffer.end();) {
        it->duration--;
        if (it->duration <= 0) {
            logs.push_back(name + "'s [" + ElementalSystem::GetElementName(it->element) + "] wore off.");
            it = statusBuffer.erase(it);
        } else {
            ++it;
        }
    }
}

void Entity::TriggerHitVisuals() {
    hitShakeTimer = 0.25f;
    flashTimer = 0.2f;
}

void Entity::UpdateVisuals(float dt) {
    if (hitShakeTimer > 0.0f) hitShakeTimer -= dt;
    if (flashTimer > 0.0f) flashTimer -= dt;
}

Vector2 Entity::GetRenderOffset() const {
    if (hitShakeTimer > 0.0f) {
        float offset = sinf(hitShakeTimer * 50.0f) * 6.0f;
        return (Vector2){ offset, 0.0f };
    }
    return (Vector2){ 0.0f, 0.0f };
}

// ==========================================
// Player Implementation
// ==========================================

Player::Player(std::string name, int maxHp)
    : Entity(name, maxHp, (Color){ 52, 152, 219, 255 }), currentStance(StanceType::ATTACK), comboScore(0) {
}

DamageReport Player::ApplyIncomingDamage(int rawDamage, Element element, StanceType receiverStance, float weatherMultiplier) {
    DamageReport report = Entity::ApplyIncomingDamage(rawDamage, element, receiverStance, weatherMultiplier);
    
    // Parry Stance Counter mechanics
    if (receiverStance == StanceType::PARRY) {
        report.wasParried = true;
    }
    return report;
}

// ==========================================
// Enemy Implementation
// ==========================================

Enemy::Enemy(std::string name, std::string type, int maxHp, Color color, int tier)
    : Entity(name, maxHp, color), enemyType(type), patternIndex(0), tier(tier) {
}

void Enemy::DecideIntent(int turnNumber, const Player& player, WeatherType currentWeather) {
    (void)player;
    (void)turnNumber;
    int step = patternIndex % 3;

    if (enemyType == "Pyromancer") {
        if (currentWeather == WeatherType::HEATWAVE) {
            currentIntent = { IntentType::ATTACK, 24, Element::FIRE, "Inferno Flare", "Heavy Fire strike boosted by heatwave!" };
        } else if (step == 0) {
            currentIntent = { IntentType::ATTACK, 14, Element::FIRE, "Firebolt", "Launches a flaming projectile." };
        } else if (step == 1) {
            currentIntent = { IntentType::DEBUFF, 0, Element::OIL, "Oil Splatter", "Drenches target in flammable oil." };
        } else {
            currentIntent = { IntentType::DEFEND, 15, Element::FIRE, "Flame Ward", "Channels a blazing protective barrier." };
        }
    } else if (enemyType == "Frost Golem") {
        if (currentWeather == WeatherType::BLIZZARD) {
            currentIntent = { IntentType::ATTACK, 20, Element::COLD, "Glacial Avalanche", "Massive frost smash empowered by blizzard!" };
        } else if (step == 0) {
            currentIntent = { IntentType::ATTACK, 15, Element::COLD, "Ice Shard", "Shoots sharp ice spikes." };
        } else if (step == 1) {
            currentIntent = { IntentType::DEFEND, 20, Element::NONE, "Glacial Armor", "Fortifies defense with solid permafrost." };
        } else {
            currentIntent = { IntentType::DEBUFF, 8, Element::COLD, "Frost Breath", "Chills the player with Cold." };
        }
    } else if (enemyType == "Storm Harpy") {
        if (currentWeather == WeatherType::THUNDERSTORM) {
            currentIntent = { IntentType::ATTACK, 22, Element::LIGHTNING, "Volt Overload", "Harnesses thunderstorm to unleash lightning!" };
        } else if (step == 0) {
            currentIntent = { IntentType::ATTACK, 16, Element::LIGHTNING, "Thunder Dive", "Swoops in with crackling lightning." };
        } else if (step == 1) {
            currentIntent = { IntentType::ATTACK, 12, Element::GALE, "Gale Whirlwind", "Unleashes wind gusts that displace air." };
        } else {
            currentIntent = { IntentType::BUFF, 10, Element::LIGHTNING, "Static Charge", "Generates crackling electric energy." };
        }
    } else if (enemyType == "Aquamancer Slime") {
        if (step == 0) {
            currentIntent = { IntentType::DEBUFF, 10, Element::WET, "Water Jet", "Blasts target with high pressure water, applying [WET]." };
        } else if (step == 1) {
            currentIntent = { IntentType::DEFEND, 14, Element::WET, "Aqua Bubble", "Forms an aquatic protective sphere." };
        } else {
            currentIntent = { IntentType::ATTACK, 12, Element::WET, "Tidal Splash", "Deals Water damage to target." };
        }
    } else { // Boss: Elemental Archon
        int bossStep = patternIndex % 4;
        if (bossStep == 0) {
            currentIntent = { IntentType::ATTACK, 26, Element::FIRE, "Hellfire Nova", "Devastating multi-elemental combustion!" };
        } else if (bossStep == 1) {
            currentIntent = { IntentType::DEBUFF, 16, Element::WET, "Deluge Vortex", "Drenches the arena in water vortex." };
        } else if (bossStep == 2) {
            currentIntent = { IntentType::ATTACK, 28, Element::LIGHTNING, "Judgment Bolt", "Calls down blinding lightning!" };
        } else {
            currentIntent = { IntentType::DEFEND, 30, Element::COLD, "Crystalline Aegis", "Erects an impenetrable frost wall." };
        }
    }
}
