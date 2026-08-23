#pragma once

#include "Common.hpp"

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

class WeatherSystem {
private:
    WeatherType currentWeather;
    std::deque<WeatherType> forecastQueue;
    int turnsElapsed;

public:
    WeatherSystem();

    void Initialize(WeatherType initialWeather = WeatherType::CLEAR);
    void AdvanceForecast();
    
    WeatherType GetCurrentWeather() const { return currentWeather; }
    const std::deque<WeatherType>& GetForecastQueue() const { return forecastQueue; }
    int GetTurnsElapsed() const { return turnsElapsed; }

    WeatherTriggerResult ProcessTurnStartWeather();

    static WeatherType GenerateRandomWeather();
    static const char* GetWeatherName(WeatherType weather);
    static const char* GetWeatherIcon(WeatherType weather);
    static const char* GetWeatherShortDesc(WeatherType weather);
    static std::string GetWeatherDetailedDesc(WeatherType weather);
    static Color GetWeatherColor(WeatherType weather);
};
