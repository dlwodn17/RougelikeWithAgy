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
    int GetTurnsElapsed() const { return turnsElapsed; }

    // Evaluates current active weather effects for combat turn resolution
    WeatherTriggerResult ProcessTurnStartWeather();

    static WeatherType GenerateRandomWeather();
    static const char* GetWeatherName(WeatherType weather);
    static const char* GetWeatherIcon(WeatherType weather);
    static const char* GetWeatherShortDesc(WeatherType weather);
    static std::string GetWeatherDetailedDesc(WeatherType weather);
    static ColorRGBA GetWeatherColor(WeatherType weather);
};
