#include "Entities/Enemy.hpp"

Enemy::Enemy(const std::string& name, const std::string& type, int maxHp, Color enemyColor, int enemyTier)
    : Entity(name, maxHp), enemyType(type), tier(enemyTier), patternIndex(0), color(enemyColor) {
}

void Enemy::DecideIntent(int turn, const Player& player, WeatherType currentWeather) {
    (void)turn;
    (void)player;

    // AI Intent logic based on type and weather
    if (enemyType == "Aquamancer Slime") {
        int cycle = patternIndex % 3;
        if (cycle == 0) {
            currentIntent.type = IntentType::ATTACK;
            currentIntent.value = 12;
            currentIntent.element = Element::WET;
            currentIntent.name = "Water Jet";
            currentIntent.desc = "Deals 12 Water DMG & applies [WET]";
        } else if (cycle == 1) {
            currentIntent.type = IntentType::DEBUFF;
            currentIntent.value = 0;
            currentIntent.element = Element::WET;
            currentIntent.name = "Soak Spray";
            currentIntent.desc = "Drenches player in [WET] for 3 turns";
        } else {
            currentIntent.type = IntentType::DEFEND;
            currentIntent.value = 14;
            currentIntent.element = Element::NONE;
            currentIntent.name = "Bubble Shield";
            currentIntent.desc = "Gains 14 Shield";
        }
    } else if (enemyType == "Pyromancer") {
        int cycle = patternIndex % 3;
        if (currentWeather == WeatherType::HEATWAVE || cycle == 0) {
            currentIntent.type = IntentType::ATTACK;
            currentIntent.value = 18;
            currentIntent.element = Element::FIRE;
            currentIntent.name = "Fireball";
            currentIntent.desc = "Deals 18 Fire DMG & applies [FIRE]";
        } else if (cycle == 1) {
            currentIntent.type = IntentType::DEBUFF;
            currentIntent.value = 0;
            currentIntent.element = Element::OIL;
            currentIntent.name = "Oil Coating";
            currentIntent.desc = "Coats player in flammable [OIL]";
        } else {
            currentIntent.type = IntentType::ATTACK;
            currentIntent.value = 14;
            currentIntent.element = Element::FIRE;
            currentIntent.name = "Flame Burst";
            currentIntent.desc = "Deals 14 Fire DMG";
        }
    } else if (enemyType == "Storm Harpy") {
        int cycle = patternIndex % 3;
        if (cycle == 0) {
            currentIntent.type = IntentType::ATTACK;
            currentIntent.value = 16;
            currentIntent.element = Element::LIGHTNING;
            currentIntent.name = "Volt Claw";
            currentIntent.desc = "Deals 16 Lightning DMG";
        } else if (cycle == 1) {
            currentIntent.type = IntentType::DEBUFF;
            currentIntent.value = 0;
            currentIntent.element = Element::GALE;
            currentIntent.name = "Gust Wing";
            currentIntent.desc = "Spreads statuses with wind";
        } else {
            currentIntent.type = IntentType::ATTACK;
            currentIntent.value = 22;
            currentIntent.element = Element::LIGHTNING;
            currentIntent.name = "Thunder Strike";
            currentIntent.desc = "Deals 22 heavy Lightning DMG";
        }
    } else if (enemyType == "Frost Golem") {
        int cycle = patternIndex % 3;
        if (cycle == 0) {
            currentIntent.type = IntentType::ATTACK;
            currentIntent.value = 15;
            currentIntent.element = Element::COLD;
            currentIntent.name = "Ice Slam";
            currentIntent.desc = "Deals 15 Cold DMG (Freezes if player is Wet)";
        } else if (cycle == 1) {
            currentIntent.type = IntentType::DEFEND;
            currentIntent.value = 20;
            currentIntent.element = Element::NONE;
            currentIntent.name = "Glacial Barrier";
            currentIntent.desc = "Fortifies with 20 Shield";
        } else {
            currentIntent.type = IntentType::DEBUFF;
            currentIntent.value = 0;
            currentIntent.element = Element::COLD;
            currentIntent.name = "Frost Breath";
            currentIntent.desc = "Inflicts [COLD] on player";
        }
    } else if (enemyType == "Elemental Archon") { // Boss
        int cycle = patternIndex % 4;
        if (cycle == 0) {
            currentIntent.type = IntentType::ATTACK;
            currentIntent.value = 24;
            currentIntent.element = Element::LIGHTNING;
            currentIntent.name = "Judgement Bolt";
            currentIntent.desc = "Devastating 24 Lightning DMG";
        } else if (cycle == 1) {
            currentIntent.type = IntentType::ATTACK;
            currentIntent.value = 20;
            currentIntent.element = Element::FIRE;
            currentIntent.name = "Inferno Cleave";
            currentIntent.desc = "Deals 20 Fire DMG & applies [FIRE]";
        } else if (cycle == 2) {
            currentIntent.type = IntentType::DEBUFF;
            currentIntent.value = 0;
            currentIntent.element = Element::OIL;
            currentIntent.name = "Primordial Ooze";
            currentIntent.desc = "Applies [OIL] preparing for explosion";
        } else {
            currentIntent.type = IntentType::DEFEND;
            currentIntent.value = 30;
            currentIntent.element = Element::NONE;
            currentIntent.name = "Aegis of Elements";
            currentIntent.desc = "Gains 30 Shield and cleanses statuses";
        }
    } else {
        currentIntent.type = IntentType::ATTACK;
        currentIntent.value = 10;
        currentIntent.element = Element::NONE;
        currentIntent.name = "Strike";
        currentIntent.desc = "Deals 10 physical damage";
    }
}

void Enemy::AdvancePattern() {
    patternIndex++;
}
