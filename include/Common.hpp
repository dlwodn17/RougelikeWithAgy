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

// ==========================================
// Display & Multi-Resolution (up to QHD) Options
// ==========================================

struct ResolutionOption {
    int width;
    int height;
    const char* label;
    const char* tag;
};

class DisplaySettings {
private:
    static inline int currentResolutionIndex = 2; // Default to 1920x1080 (FHD) for safety, switchable to QHD

public:
    static const std::vector<ResolutionOption>& GetResolutions() {
        static const std::vector<ResolutionOption> resList = {
            { 1280, 720,  "1280 x 720",  "HD (720p)" },
            { 1600, 900,  "1600 x 900",  "HD+ (900p)" },
            { 1920, 1080, "1920 x 1080", "FHD (1080p)" },
            { 2560, 1440, "2560 x 1440", "QHD (1440p) ★" }
        };
        return resList;
    }

    static void AutoDetectDefaultResolution() {
        int monitor = GetCurrentMonitor();
        int monW = GetMonitorWidth(monitor);
        int monH = GetMonitorHeight(monitor);

        if (monW >= 2560 && monH >= 1440) {
            currentResolutionIndex = 3; // QHD
        } else if (monW >= 1920 && monH >= 1080) {
            currentResolutionIndex = 2; // FHD
        } else if (monW >= 1600 && monH >= 900) {
            currentResolutionIndex = 1; // HD+
        } else {
            currentResolutionIndex = 0; // HD
        }
    }

    static int GetCurrentResolutionIndex() {
        return currentResolutionIndex;
    }

    static void SetResolutionIndex(int index) {
        const auto& list = GetResolutions();
        if (index >= 0 && index < static_cast<int>(list.size())) {
            currentResolutionIndex = index;
            if (!IsWindowFullscreen()) {
                SetWindowSize(list[index].width, list[index].height);
                int monitor = GetCurrentMonitor();
                int monW = GetMonitorWidth(monitor);
                int monH = GetMonitorHeight(monitor);
                SetWindowPosition((monW - list[index].width) / 2, (monH - list[index].height) / 2);
            }
        }
    }

    static void CycleResolution(int delta) {
        const auto& list = GetResolutions();
        int count = static_cast<int>(list.size());
        int newIdx = (currentResolutionIndex + delta) % count;
        if (newIdx < 0) newIdx += count;
        SetResolutionIndex(newIdx);
    }

    static void ToggleFullscreenMode() {
        ::ToggleFullscreen();
        if (!IsWindowFullscreen()) {
            const auto& list = GetResolutions();
            SetWindowSize(list[currentResolutionIndex].width, list[currentResolutionIndex].height);
            int monitor = GetCurrentMonitor();
            int monW = GetMonitorWidth(monitor);
            int monH = GetMonitorHeight(monitor);
            SetWindowPosition((monW - list[currentResolutionIndex].width) / 2, (monH - list[currentResolutionIndex].height) / 2);
        }
    }

    static bool IsFullscreenMode() {
        return IsWindowFullscreen();
    }
};

struct ScreenConfig {
    static constexpr int VIRTUAL_WIDTH = 2560;
    static constexpr int VIRTUAL_HEIGHT = 1440;

    static Vector2 GetVirtualMousePosition() {
        Vector2 mouse = ::GetMousePosition();
        int screenW = ::GetScreenWidth();
        int screenH = ::GetScreenHeight();
        if (screenW <= 0 || screenH <= 0) return mouse;
        
        float scale = std::min((float)screenW / (float)VIRTUAL_WIDTH, (float)screenH / (float)VIRTUAL_HEIGHT);
        if (scale <= 0.0001f) scale = 1.0f;

        float offsetX = ((float)screenW - ((float)VIRTUAL_WIDTH * scale)) * 0.5f;
        float offsetY = ((float)screenH - ((float)VIRTUAL_HEIGHT * scale)) * 0.5f;
        return (Vector2){ (mouse.x - offsetX) / scale, (mouse.y - offsetY) / scale };
    }

    static void ToggleGameFullscreen() {
        DisplaySettings::ToggleFullscreenMode();
    }
};


