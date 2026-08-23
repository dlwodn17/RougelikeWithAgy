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
    PLAYER_INPUT = 0,       // Waiting for player skill/stance selection
    ACTION_EXECUTION,       // Player skill animation & damage/reactions
    WEATHER_TRIGGER,        // Weather triggers start-of-turn or end-of-turn effects
    ENEMY_ACTIONS,          // Enemies execute intents sequentially
    STATUS_TICK,            // Burn DoT, Frozen thaw, shield decays
    TURN_END_CLEANUP,       // Cooldown ticks, forecast shifts
    VICTORY_SCREEN,         // Wave cleared
    DEFEAT_SCREEN           // Player defeated
};

enum class AppState {
    TITLE = 0,
    BATTLE,
    VICTORY,
    GAME_OVER,
    TUTORIAL_MODAL
};

// ==========================================
// Structs
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

// ==========================================
// Color & Icon Helpers
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
