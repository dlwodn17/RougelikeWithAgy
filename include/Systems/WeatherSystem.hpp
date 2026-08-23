#pragma once

#include "Core/Types.hpp"

class WeatherSystem {
private:
    WeatherType currentWeather;
    std::deque<WeatherType> forecastQueue;
    int turnsElapsed;

public:
    WeatherSystem();

    void Initialize(WeatherType initialWeather = WeatherType::CLEAR);
    void Reset(WeatherType initialWeather = WeatherType::CLEAR) { Initialize(initialWeather); }
    WeatherType AdvanceTurn();
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
    static ColorRGBA GetWeatherColor(WeatherType weather);
};
