#include "Systems/WeatherSystem.hpp"
#include <random>

static std::mt19937 weatherRng(4242);

WeatherSystem::WeatherSystem() : currentWeather(WeatherType::CLEAR), turnsElapsed(0) {
    Initialize(WeatherType::CLEAR);
}

void WeatherSystem::Initialize(WeatherType initialWeather) {
    currentWeather = initialWeather;
    turnsElapsed = 0;
    forecastQueue.clear();

    // Populate initial 3-turn forecast queue
    for (int i = 0; i < 3; ++i) {
        forecastQueue.push_back(GenerateRandomWeather());
    }
}

WeatherType WeatherSystem::GenerateRandomWeather() {
    // Weather table: CLEAR, RAIN, HEATWAVE, GALE, STORM, BLIZZARD, ACID_RAIN
    std::uniform_int_distribution<int> dist(0, 5);
    int pick = dist(weatherRng);
    switch (pick) {
        case 0: return WeatherType::RAIN;
        case 1: return WeatherType::HEATWAVE;
        case 2: return WeatherType::STORM;
        case 3: return WeatherType::BLIZZARD;
        case 4: return WeatherType::GALE;
        case 5: return WeatherType::ACID_RAIN;
        default: return WeatherType::CLEAR;
    }
}

WeatherType WeatherSystem::AdvanceTurn() {
    turnsElapsed++;
    if (!forecastQueue.empty()) {
        currentWeather = forecastQueue.front();
        forecastQueue.pop_front();
    } else {
        currentWeather = GenerateRandomWeather();
    }

    // Always maintain 3 forecast items in queue
    while (forecastQueue.size() < 3) {
        forecastQueue.push_back(GenerateRandomWeather());
    }
    return currentWeather;
}

WeatherTriggerResult WeatherSystem::ProcessTurnStartWeather() {
    WeatherTriggerResult result;
    result.title = GetWeatherName(currentWeather);
    result.weatherColor = GetWeatherColor(currentWeather);

    switch (currentWeather) {
        case WeatherType::RAIN:
            result.description = "Torrential downpour drenches all combatants with [WET]!";
            result.globalStatusToApply = Element::WET;
            result.statusDuration = 2;
            result.waterDamageModifier = 1.35f;
            result.fireDamageModifier = 0.70f;
            break;

        case WeatherType::HEATWAVE:
            result.description = "Blistering heat intensifies flame combustion! (+50% Fire DMG)";
            result.fireDamageModifier = 1.50f;
            result.waterDamageModifier = 0.80f;
            break;

        case WeatherType::STORM:
            result.description = "Severe thunderstorm drenches everyone [WET] and calls down 15 lightning damage!";
            result.globalStatusToApply = Element::WET;
            result.statusDuration = 2;
            result.strikeRandomEnemy = true;
            result.randomStrikeDamage = 15;
            result.lightningDamageModifier = 1.30f;
            break;

        case WeatherType::BLIZZARD:
            result.description = "Freezing temperatures apply [COLD] to all combatants. Drenched entities freeze solid!";
            result.globalStatusToApply = Element::COLD;
            result.statusDuration = 2;
            result.coldDamageModifier = 1.30f;
            result.fireDamageModifier = 0.80f;
            break;

        case WeatherType::GALE:
            result.description = "Violent windstorms swirl active elemental statuses across all combatants!";
            result.spreadAllDebuffs = true;
            break;

        case WeatherType::ACID_RAIN:
            result.description = "Corrosive chemical rainfall drenches everyone with flammable [OIL]!";
            result.globalStatusToApply = Element::OIL;
            result.statusDuration = 2;
            break;

        case WeatherType::CLEAR:
        default:
            result.description = "Calm atmosphere. No active environmental modifiers.";
            break;
    }

    return result;
}

const char* WeatherSystem::GetWeatherName(WeatherType weather) {
    return GetWeatherTitleStr(weather);
}

const char* WeatherSystem::GetWeatherIcon(WeatherType weather) {
    switch (weather) {
        case WeatherType::RAIN:         return "[RAIN] Rain";
        case WeatherType::HEATWAVE:     return "[HEAT] Heatwave";
        case WeatherType::STORM:        return "[STORM] Storm";
        case WeatherType::BLIZZARD:     return "[SNOW] Blizzard";
        case WeatherType::GALE:         return "[GALE] Gale";
        case WeatherType::ACID_RAIN:    return "[ACID] Acid Rain";
        case WeatherType::CLEAR:
        default:                        return "[SUN] Clear";
    }
}

const char* WeatherSystem::GetWeatherShortDesc(WeatherType weather) {
    switch (weather) {
        case WeatherType::RAIN:         return "Applies [WET] / Boosts Water";
        case WeatherType::HEATWAVE:     return "+50% Fire DMG bonus";
        case WeatherType::STORM:        return "[WET] + 15 Lightning strike";
        case WeatherType::BLIZZARD:     return "Applies [COLD] / Freezes Wet";
        case WeatherType::GALE:         return "Spreads statuses across units";
        case WeatherType::ACID_RAIN:    return "Applies reactive [OIL]";
        case WeatherType::CLEAR:
        default:                        return "Standard combat";
    }
}

std::string WeatherSystem::GetWeatherDetailedDesc(WeatherType weather) {
    switch (weather) {
        case WeatherType::RAIN:
            return "Applies [WET] to all entities each turn. Increases Water damage by 35% and dampens Fire.";
        case WeatherType::HEATWAVE:
            return "Intense dry heat amplifies all Fire skill damage by 50%.";
        case WeatherType::STORM:
            return "Heavy storm applies [WET] globally and strikes a random enemy with a 15 damage lightning bolt.";
        case WeatherType::BLIZZARD:
            return "Sub-zero blizzard applies [COLD] to all entities. Any entity already [WET] freezes solid for 1 turn.";
        case WeatherType::GALE:
            return "Gusts of wind spread all active elemental debuffs between neighboring enemies.";
        case WeatherType::ACID_RAIN:
            return "Corrosive rain applies [OIL] to all targets, making them highly vulnerable to explosive ignition.";
        case WeatherType::CLEAR:
        default:
            return "Clear skies with no special weather effects.";
    }
}

ColorRGBA WeatherSystem::GetWeatherColor(WeatherType weather) {
    switch (weather) {
        case WeatherType::RAIN:         return { 52, 152, 219, 255 };  // Blue
        case WeatherType::HEATWAVE:     return { 230, 126, 34, 255 };  // Orange
        case WeatherType::STORM:        return { 142, 68, 173, 255 };  // Purple
        case WeatherType::BLIZZARD:     return { 129, 236, 236, 255 }; // Ice Cyan
        case WeatherType::GALE:         return { 46, 204, 113, 255 };  // Green
        case WeatherType::ACID_RAIN:    return { 108, 92, 231, 255 };  // Violet
        case WeatherType::CLEAR:
        default:                        return { 241, 196, 15, 255 };  // Gold
    }
}
