#include "WeatherSystem.hpp"
#include <random>

static std::mt19937& GetRNG() {
    static std::mt19937 rng(42);
    return rng;
}

WeatherSystem::WeatherSystem() : currentWeather(WeatherType::CLEAR), turnsElapsed(0) {
    Initialize(WeatherType::CLEAR);
}

void WeatherSystem::Initialize(WeatherType initialWeather) {
    currentWeather = initialWeather;
    forecastQueue.clear();
    turnsElapsed = 0;

    // Fill forecast queue with 3 upcoming weather conditions
    for (int i = 0; i < 3; ++i) {
        forecastQueue.push_back(GenerateRandomWeather());
    }
}

WeatherType WeatherSystem::GenerateRandomWeather() {
    std::uniform_int_distribution<int> dist(0, 6);
    int roll = dist(GetRNG());
    switch (roll) {
        case 0: return WeatherType::CLEAR;
        case 1: return WeatherType::RAIN;
        case 2: return WeatherType::HEATWAVE;
        case 3: return WeatherType::THUNDERSTORM;
        case 4: return WeatherType::BLIZZARD;
        case 5: return WeatherType::GALE_WINDS;
        case 6: return WeatherType::ACID_RAIN;
        default: return WeatherType::CLEAR;
    }
}

void WeatherSystem::AdvanceForecast() {
    turnsElapsed++;
    if (!forecastQueue.empty()) {
        currentWeather = forecastQueue.front();
        forecastQueue.pop_front();
    } else {
        currentWeather = GenerateRandomWeather();
    }

    // Always maintain 3 forecast items
    while (forecastQueue.size() < 3) {
        forecastQueue.push_back(GenerateRandomWeather());
    }
}

WeatherTriggerResult WeatherSystem::ProcessTurnStartWeather() {
    WeatherTriggerResult result;
    result.title = GetWeatherName(currentWeather);
    result.weatherColor = GetWeatherColor(currentWeather);

    switch (currentWeather) {
        case WeatherType::CLEAR:
            result.description = "Skies are clear. No passive weather effects.";
            break;

        case WeatherType::RAIN:
            result.description = "Torrential rain drenches the battlefield, applying [WET] to everyone!";
            result.globalStatusToApply = Element::WET;
            result.statusDuration = 2;
            result.waterDamageModifier = 1.35f;
            result.fireDamageModifier = 0.65f;
            break;

        case WeatherType::HEATWAVE:
            result.description = "Scorching heat intensifies flames! +50% Fire damage.";
            result.globalStatusToApply = Element::NONE;
            result.fireDamageModifier = 1.50f;
            result.waterDamageModifier = 0.75f;
            break;

        case WeatherType::THUNDERSTORM:
            result.description = "Heavy rain applies [WET]; lightning violently strikes a target for 15 damage!";
            result.globalStatusToApply = Element::WET;
            result.statusDuration = 2;
            result.strikeRandomEnemy = true;
            result.randomStrikeDamage = 15;
            result.lightningDamageModifier = 1.30f;
            break;

        case WeatherType::BLIZZARD:
            result.description = "Freezing blizzard chills all combatants with [COLD]. Wet units freeze!";
            result.globalStatusToApply = Element::COLD;
            result.statusDuration = 2;
            result.coldDamageModifier = 1.35f;
            break;

        case WeatherType::GALE_WINDS:
            result.description = "Turbulent gale-force winds swirl across the field, spreading active statuses!";
            result.spreadAllDebuffs = true;
            break;

        case WeatherType::ACID_RAIN:
            result.description = "Corrosive downpour coats all units with flammable [OIL]!";
            result.globalStatusToApply = Element::OIL;
            result.statusDuration = 2;
            break;
    }

    return result;
}

const char* WeatherSystem::GetWeatherName(WeatherType weather) {
    return GetWeatherTitle(weather);
}

const char* WeatherSystem::GetWeatherIcon(WeatherType weather) {
    switch (weather) {
        case WeatherType::CLEAR:        return "☀️ CLEAR";
        case WeatherType::RAIN:         return "🌧️ RAIN";
        case WeatherType::HEATWAVE:     return "🔥 HEATWAVE";
        case WeatherType::THUNDERSTORM: return "⛈️ THUNDER";
        case WeatherType::BLIZZARD:     return "🌨️ BLIZZARD";
        case WeatherType::GALE_WINDS:   return "🌪️ GALE";
        case WeatherType::ACID_RAIN:    return "🧪 ACID RAIN";
        default:                        return "☀️ CLEAR";
    }
}

const char* WeatherSystem::GetWeatherShortDesc(WeatherType weather) {
    switch (weather) {
        case WeatherType::CLEAR:        return "Standard conditions";
        case WeatherType::RAIN:         return "Applies WET, +35% Water DMG";
        case WeatherType::HEATWAVE:     return "+50% Fire DMG";
        case WeatherType::THUNDERSTORM: return "Applies WET, 15 Lightning Strike";
        case WeatherType::BLIZZARD:     return "Applies COLD, Freezes WET targets";
        case WeatherType::GALE_WINDS:   return "Spreads all active debuffs";
        case WeatherType::ACID_RAIN:    return "Applies OIL to all combatants";
        default:                        return "Standard conditions";
    }
}

std::string WeatherSystem::GetWeatherDetailedDesc(WeatherType weather) {
    switch (weather) {
        case WeatherType::CLEAR:
            return "Calm weather with no active modifiers.";
        case WeatherType::RAIN:
            return "Drenches all combatants in [WET] at turn start. Boosts Water damage by 35%, dampens Fire.";
        case WeatherType::HEATWAVE:
            return "Extreme heat boosts Fire damage by 50% and accelerates status ticks.";
        case WeatherType::THUNDERSTORM:
            return "Drenches all in [WET] and calls down a random 15-damage lightning strike.";
        case WeatherType::BLIZZARD:
            return "Inflicts [COLD] on all entities. Any entity already [WET] will instantly trigger [FROZEN]!";
        case WeatherType::GALE_WINDS:
            return "Howling winds carry element particles across combatants, spreading active debuffs.";
        case WeatherType::ACID_RAIN:
            return "Coats everyone in highly reactive [OIL], preparing them for explosive combustion.";
        default:
            return "Normal atmospheric conditions.";
    }
}

Color WeatherSystem::GetWeatherColor(WeatherType weather) {
    switch (weather) {
        case WeatherType::CLEAR:        return (Color){ 241, 196, 15, 255 };  // Sun Gold
        case WeatherType::RAIN:         return (Color){ 41, 128, 185, 255 };  // Rain Blue
        case WeatherType::HEATWAVE:     return (Color){ 230, 126, 34, 255 };  // Sun Heat Orange
        case WeatherType::THUNDERSTORM: return (Color){ 142, 68, 173, 255 };  // Storm Violet
        case WeatherType::BLIZZARD:     return (Color){ 129, 236, 236, 255 }; // Blizzard Ice Cyan
        case WeatherType::GALE_WINDS:   return (Color){ 39, 174, 96, 255 };   // Gale Green
        case WeatherType::ACID_RAIN:    return (Color){ 108, 92, 231, 255 };  // Acid Sludge
        default:                        return WHITE;
    }
}
