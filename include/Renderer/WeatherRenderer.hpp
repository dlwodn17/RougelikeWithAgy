#pragma once

#include "Core/Constants.hpp"
#include "Systems/WeatherSystem.hpp"

class WeatherRenderer {
public:
    // Draws atmospheric background color gradient based on current weather
    static void DrawWeatherBackground(WeatherType weather);

    // Draws top center weather forecast queue panel (Current + Next 3 Turns)
    static void DrawForecastPanel(const WeatherSystem& weatherSystem);

    // Draws a single weather card/badge
    static void DrawWeatherBadge(Rectangle rec, WeatherType weather, const char* prefix, bool isActive = false);
};
