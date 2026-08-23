#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <deque>
#include <memory>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <unordered_map>

// ============================================================================
// A. Element Bitflags & Bitwise Operators (Pure C++17, Zero Engine Dependency)
// ============================================================================

enum class Element : uint32_t {
    NONE        = 0,
    WET         = 1 << 0,   // 1  (Water / Moisture)
    FIRE        = 1 << 1,   // 2  (Combustion / Heat)
    OIL         = 1 << 2,   // 4  (Flammable Sludge / Chemical)
    LIGHTNING   = 1 << 3,   // 8  (Electricity / Shock)
    COLD        = 1 << 4,   // 16 (Ice / Frost)
    GALE        = 1 << 5    // 32 (Wind / Dispersal)
};

// Bitwise Operator Overloads for Element Bitflags
inline constexpr Element operator|(Element lhs, Element rhs) {
    return static_cast<Element>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
}

inline constexpr Element operator&(Element lhs, Element rhs) {
    return static_cast<Element>(static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs));
}

inline constexpr Element operator^(Element lhs, Element rhs) {
    return static_cast<Element>(static_cast<uint32_t>(lhs) ^ static_cast<uint32_t>(rhs));
}

inline constexpr Element operator~(Element elem) {
    return static_cast<Element>(~static_cast<uint32_t>(elem));
}

inline Element& operator|=(Element& lhs, Element rhs) {
    lhs = lhs | rhs;
    return lhs;
}

inline Element& operator&=(Element& lhs, Element rhs) {
    lhs = lhs & rhs;
    return lhs;
}

inline Element& operator^=(Element& lhs, Element rhs) {
    lhs = lhs ^ rhs;
    return lhs;
}

inline bool HasFlag(Element mask, Element flag) {
    return (static_cast<uint32_t>(mask) & static_cast<uint32_t>(flag)) == static_cast<uint32_t>(flag);
}

// ============================================================================
// Core Enums
// ============================================================================

enum class ReactionType {
    NONE = 0,
    SHOCK,      // WET + LIGHTNING -> High Voltage Chain AoE + Burst
    EXPLOSION,  // OIL + FIRE -> Cataclysmic Burst + Burning DoT
    FROZEN,     // WET + COLD -> Immobilization (Skip 1 Turn)
    MELT,       // FIRE + COLD -> Superheated Steam Vaporization Bonus
    PLASMA,     // OIL + LIGHTNING -> Armor-Piercing Discharge
    SPREAD      // GALE + ANY -> Spreads Debuffs to Neighbors
};

enum class WeatherType {
    CLEAR = 0,      // Normal atmospheric baseline
    RAIN,           // Applies WET globally; amplifies Water damage
    HEATWAVE,       // Amplifies FIRE damage (+50%); applies Burning
    GALE,           // Swirls and spreads debuffs to adjacent targets
    STORM,          // Heavy rain (WET) + random lightning bolts (15 dmg)
    BLIZZARD,       // Applies COLD; flash-freezes WET targets; Cold dmg +30%
    ACID_RAIN       // Drenches all units in combustible OIL
};

// Aliases for compatibility
inline constexpr WeatherType THUNDERSTORM = WeatherType::STORM;
inline constexpr WeatherType GALE_WINDS = WeatherType::GALE;

enum class StanceType {
    ATTACK = 0,     // +40% Outgoing DMG
    DEFENSE,        // +18 Shield, -30% Incoming DMG
    PARRY           // -50% Incoming DMG, reflects debuffs & counters
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
    PLAYER_INPUT = 0,               // Step 1: Waiting for player skill/stance choice
    RESOLVE_PLAYER,                 // Step 2: Resolve player action & elemental reactions
    APPLY_WEATHER,                  // Step 3: Apply environmental active weather effects
    ENEMY_TURN,                     // Step 4: Resolve enemy AI action & player stance/parry
    END_ROUND_TICK,                 // Step 5: DoT tick, Cooldown -1, Weather queue advance, start next turn
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

// ============================================================================
// Core Structs
// ============================================================================

struct StatusInstance {
    Element element;
    int duration;   // Turns remaining
    int stacks;     // Number of stacks

    StatusInstance(Element elem = Element::NONE, int dur = 2, int stk = 1)
        : element(elem), duration(dur), stacks(stk) {}
};

struct Intent {
    IntentType type = IntentType::ATTACK;
    int value = 0;              // Damage, shield, or heal amount
    Element element = Element::NONE;
    std::string name = "Attack";
    std::string desc = "Deals damage";
};

// Return structure for Elemental Reactions
struct ReactionResult {
    bool triggered = false;
    ReactionType type = ReactionType::NONE;
    std::string name = "";
    std::string description = "";
    int bonusDamage = 0;
    float damageMultiplier = 1.0f;
    Element consumedElements = Element::NONE; // Elements consumed by reaction
    Element appliedElements = Element::NONE;  // New element inflicted (e.g., Burn from Explosion)
    int appliedDuration = 0;
    bool chainAoE = false;
    int aoeDamage = 0;
    bool stunTarget = false;                  // E.g., Frozen
    bool isParried = false;
};

using ReactionOutcome = ReactionResult;

struct DamageReport {
    int rawDamage = 0;
    int mitigatedDamage = 0;
    int shieldAbsorbed = 0;
    int healthDamage = 0;
    bool wasParried = false;
    bool wasShielded = false;
    bool wasCritical = false;
    bool causedDeath = false;
    ReactionResult reaction;
};

struct Vec2 {
    float x = 0.0f;
    float y = 0.0f;
};

struct ColorRGBA {
    uint8_t r = 255;
    uint8_t g = 255;
    uint8_t b = 255;
    uint8_t a = 255;
};

struct FloatingText {
    Vec2 position;
    Vec2 velocity;
    std::string text;
    ColorRGBA color;
    float lifetime = 1.2f;
    float maxLifetime = 1.2f;
    float fontSize = 20.0f;
    float alpha = 1.0f;
};

struct CombatLogEntry {
    std::string text;
    ColorRGBA color;
    float timestamp = 0.0f;
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
    ColorRGBA weatherColor = { 255, 255, 255, 255 };
};

// ============================================================================
// Helper Utilities
// ============================================================================

inline const char* GetElementNameStr(Element elem) {
    switch (elem) {
        case Element::WET:       return "Wet";
        case Element::FIRE:      return "Fire";
        case Element::OIL:       return "Oil";
        case Element::LIGHTNING: return "Lightning";
        case Element::COLD:      return "Cold";
        case Element::GALE:      return "Gale";
        case Element::NONE:
        default:                 return "None";
    }
}

inline const char* GetElementSymbolStr(Element elem) {
    switch (elem) {
        case Element::WET:       return "💧 WET";
        case Element::FIRE:      return "🔥 FIRE";
        case Element::OIL:       return "🛢️ OIL";
        case Element::LIGHTNING: return "⚡ LIGHTNING";
        case Element::COLD:      return "❄️ COLD";
        case Element::GALE:      return "🌪️ GALE";
        case Element::NONE:
        default:                 return "⚪ NONE";
    }
}

inline ColorRGBA GetElementColorRGBA(Element elem) {
    switch (elem) {
        case Element::WET:       return { 52, 152, 219, 255 };  // Bright Blue
        case Element::FIRE:      return { 231, 76, 60, 255 };   // Flame Red
        case Element::OIL:       return { 108, 92, 231, 255 };  // Sludge Purple
        case Element::LIGHTNING: return { 241, 196, 15, 255 };  // Electric Gold
        case Element::COLD:      return { 162, 222, 255, 255 }; // Ice Cyan
        case Element::GALE:      return { 46, 204, 113, 255 };  // Emerald Green
        case Element::NONE:
        default:                 return { 189, 195, 199, 255 }; // Silver Gray
    }
}

inline const char* GetWeatherTitleStr(WeatherType w) {
    switch (w) {
        case WeatherType::CLEAR:        return "Clear Sky";
        case WeatherType::RAIN:         return "Downpour Rain";
        case WeatherType::HEATWAVE:     return "Scorching Heatwave";
        case WeatherType::GALE:         return "Howling Gale";
        case WeatherType::STORM:        return "Thunderstorm";
        case WeatherType::BLIZZARD:     return "Glacial Blizzard";
        case WeatherType::ACID_RAIN:    return "Corrosive Acid Rain";
        default:                        return "Calm";
    }
}
