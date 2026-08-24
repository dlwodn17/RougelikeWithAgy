#pragma once

#include "Core/Types.hpp"
#include <deque>
#include <string>

class WeatherSystem {
private:
    WeatherType currentWeather;
    std::deque<WeatherType> forecastQueue;
    int turnsElapsed;

public:
    WeatherSystem();

    void Initialize(WeatherType initialWeather = WeatherType::CLEAR);
    void Reset(WeatherType initialWeather = WeatherType::CLEAR) { Initialize(initialWeather); }
    
    // Advances turn: pops next weather from queue and pushes new randomized forecast
    WeatherType AdvanceTurn();
    void AdvanceForecast() { AdvanceTurn(); }
    
    WeatherType GetCurrentWeather() const { return currentWeather; }
    void SetCurrentWeather(WeatherType weather) { currentWeather = weather; }
    const std::deque<WeatherType>& GetForecastQueue() const { return forecastQueue; }
    std::deque<WeatherType>& GetForecastQueue() { return forecastQueue; }
    void SetForecastQueue(const std::deque<WeatherType>& queue) { forecastQueue = queue; }
    int GetTurnsElapsed() const { return turnsElapsed; }

    static Element GetWeatherElement(WeatherType weather) {
        switch (weather) {
            case WeatherType::RAIN:     return Element::WET;
            case WeatherType::HEATWAVE: return Element::FIRE;
            case WeatherType::BLIZZARD: return Element::COLD;
            case WeatherType::STORM:    return Element::LIGHTNING;
            default:                    return Element::NONE;
        }
    }

    Element GetWeatherSynergyElement() const {
        return GetWeatherElement(currentWeather);
    }

    // Evaluates current active weather effects for combat turn resolution
    WeatherTriggerResult ProcessTurnStartWeather();

    static WeatherType GenerateRandomWeather();
    static const char* GetWeatherName(WeatherType weather);
    static const char* GetWeatherIcon(WeatherType weather);
    static const char* GetWeatherShortDesc(WeatherType weather);
    static std::string GetWeatherDetailedDesc(WeatherType weather);
    static ColorRGBA GetWeatherColor(WeatherType weather);
};
