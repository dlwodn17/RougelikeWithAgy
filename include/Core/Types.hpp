#pragma once

#include "raylib.h"
#include <string>
#include <vector>
#include <deque>
#include <memory>
#include <functional>
#include <algorithm>
#include <cmath>
#include <iostream>

// ==========================================
// Core Enums
// ==========================================

enum class Element {
    NONE = 0,
    WET,        // Water / Moisture
    FIRE,       // Burning / Heat
    LIGHTNING,  // Shock / Electricity
    COLD,       // Ice / Frost
    OIL,        // Flammable liquid / Fuel
    GALE        // Wind / Dispersal
};

enum class WeatherType {
    CLEAR = 0,      // Normal conditions
    RAIN,           // Applies WET to all combatants every turn; boosts water
    HEATWAVE,       // Amplifies FIRE dmg +50%; applies Burn/Fire
    THUNDERSTORM,   // Heavy rain (WET) + random LIGHTNING strikes (15 dmg)
    BLIZZARD,       // Applies COLD; freezes WET entities; boosts cold dmg +30%
    GALE_WINDS,     // Spreads active debuffs to nearby entities; evasion up
    ACID_RAIN       // Applies OIL & shreds defense/shield
};

enum class StanceType {
    ATTACK = 0,     // +40% Outgoing Damage, +1 status stack buildup
    DEFENSE,        // +18 Shield, -30% Incoming Damage
    PARRY           // 50% Dmg reduction; reflects incoming statuses & deals counter damage
};

enum class TargetType {
    SINGLE_ENEMY = 0,
    ALL_ENEMIES,
    SELF,
    ALL_ALLIES
};

enum class IntentType {
    ATTACK = 0,
    DEFEND,
    DEBUFF,
    BUFF,
    SPECIAL
};

enum class CombatPhase {
    PLAYER_INPUT = 0,               // Waiting for player skill/stance selection
    RESOLVE_PLAYER_ACTION,          // Step 1: Resolve Player Action & Elemental Reactions
    RESOLVE_ENEMY_ACTIONS,          // Step 2: Resolve Enemy AI Action & Reactions
    TICK_STATUS_EFFECTS,            // Step 3: Tick Status Effects / Buffs / Debuffs on all Entities
    TICK_COOLDOWNS,                 // Step 4: Tick Player & Enemy Skill Cooldowns
    ADVANCE_WEATHER_AND_APPLY,      // Step 5: Advance Weather Forecast Queue & Apply Environmental Effect
    RESET_TURN_AND_START_NEXT,      // Step 6: Reset Stances / Action Points and start Next Turn
    VICTORY_SCREEN,                 // Wave cleared
    DEFEAT_SCREEN                   // Player defeated
};

using TurnState = CombatPhase;

enum class AppState {
    TITLE = 0,
    BATTLE,
    VICTORY,
    GAME_OVER,
    SETTINGS,
    TUTORIAL_MODAL
};

using GameScene = AppState;

// ==========================================
// Core Data Structs
// ==========================================

struct StatusInstance {
    Element element;
    int duration;   // Turns remaining
    int stacks;     // Number of stacks (if applicable)

    StatusInstance(Element elem = Element::NONE, int dur = 2, int stk = 1)
        : element(elem), duration(dur), stacks(stk) {}
};

struct Intent {
    IntentType type = IntentType::ATTACK;
    int value = 0;              // Damage or shield value
    Element element = Element::NONE;
    std::string name = "Attack";
    std::string desc = "Deals damage";
};

struct ReactionOutcome {
    bool triggered = false;
    std::string reactionName = "";
    std::string description = "";
    Color reactionColor = WHITE;
    int bonusDamage = 0;
    bool chainAoE = false;
    int aoeDamage = 0;
    bool stunTarget = false;
    bool applyBurn = false;
    int burnDuration = 0;
};

struct DamageReport {
    int rawDamage = 0;
    int mitigatedDamage = 0;
    int shieldAbsorbed = 0;
    int healthDamage = 0;
    bool wasParried = false;
    bool wasShielded = false;
    bool wasCritical = false;
    bool causedDeath = false;
    ReactionOutcome reaction;
};

struct FloatingText {
    Vector2 position;
    Vector2 velocity;
    std::string text;
    Color color;
    float lifetime;
    float maxLifetime;
    float fontSize;
    float alpha;
};

struct CombatLogEntry {
    std::string text;
    Color color;
    float timestamp;
};

struct WeatherTriggerResult {
    std::string title;
    std::string description;
    Element globalStatusToApply = Element::NONE;
    int statusDuration = 2;
    int globalDamage = 0;
    Element damageElement = Element::NONE;
    bool strikeRandomEnemy = false;
    int randomStrikeDamage = 0;
    bool spreadAllDebuffs = false;
    float fireDamageModifier = 1.0f;
    float waterDamageModifier = 1.0f;
    float lightningDamageModifier = 1.0f;
    float coldDamageModifier = 1.0f;
    Color weatherColor = WHITE;
};

// ==========================================
// Element & Weather Color/Text Helpers
// ==========================================

inline Color GetElementBaseColor(Element elem) {
    switch (elem) {
        case Element::WET:       return (Color){ 52, 152, 219, 255 };  // Bright Blue
        case Element::FIRE:      return (Color){ 231, 76, 60, 255 };   // Vibrant Red-Orange
        case Element::LIGHTNING: return (Color){ 241, 196, 15, 255 };  // Electric Gold
        case Element::COLD:      return (Color){ 162, 222, 255, 255 }; // Cyan-Ice
        case Element::OIL:       return (Color){ 108, 92, 231, 255 };  // Deep Violet / Sludge
        case Element::GALE:      return (Color){ 46, 204, 113, 255 };  // Emerald Green
        case Element::NONE:
        default:                 return (Color){ 189, 195, 199, 255 }; // Light Gray
    }
}

inline const char* GetElementSymbol(Element elem) {
    switch (elem) {
        case Element::WET:       return "[WET]";
        case Element::FIRE:      return "[FIRE]";
        case Element::LIGHTNING: return "[ELEC]";
        case Element::COLD:      return "[COLD]";
        case Element::OIL:       return "[OIL]";
        case Element::GALE:      return "[WIND]";
        case Element::NONE:
        default:                 return "[-]";
    }
}

inline const char* GetWeatherTitle(WeatherType w) {
    switch (w) {
        case WeatherType::CLEAR:        return "Clear Sky";
        case WeatherType::RAIN:         return "Downpour Rain";
        case WeatherType::HEATWAVE:     return "Scorching Heatwave";
        case WeatherType::THUNDERSTORM: return "Thunderstorm";
        case WeatherType::BLIZZARD:     return "Glacial Blizzard";
        case WeatherType::GALE_WINDS:   return "Howling Gale";
        case WeatherType::ACID_RAIN:    return "Corrosive Acid Rain";
        default:                        return "Calm";
    }
}
