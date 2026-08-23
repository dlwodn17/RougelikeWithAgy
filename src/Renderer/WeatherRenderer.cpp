#include "Renderer/WeatherRenderer.hpp"
#include <string>

void WeatherRenderer::DrawWeatherBackground(WeatherType weather) {
    int w = ScreenConfig::VIRTUAL_WIDTH;
    int h = ScreenConfig::VIRTUAL_HEIGHT;

    Color topCol = (Color){ 15, 20, 30, 255 };
    Color botCol = (Color){ 25, 30, 45, 255 };

    switch (weather) {
        case WeatherType::HEATWAVE:
            topCol = (Color){ 45, 20, 18, 255 };
            botCol = (Color){ 55, 28, 22, 255 };
            break;
        case WeatherType::BLIZZARD:
            topCol = (Color){ 14, 28, 48, 255 };
            botCol = (Color){ 22, 40, 65, 255 };
            break;
        case WeatherType::STORM:
            topCol = (Color){ 25, 16, 42, 255 };
            botCol = (Color){ 34, 22, 55, 255 };
            break;
        case WeatherType::RAIN:
            topCol = (Color){ 16, 26, 40, 255 };
            botCol = (Color){ 22, 36, 55, 255 };
            break;
        case WeatherType::GALE:
            topCol = (Color){ 18, 32, 28, 255 };
            botCol = (Color){ 24, 44, 38, 255 };
            break;
        case WeatherType::ACID_RAIN:
            topCol = (Color){ 30, 16, 40, 255 };
            botCol = (Color){ 40, 22, 54, 255 };
            break;
        case WeatherType::CLEAR:
        default:
            topCol = (Color){ 18, 22, 34, 255 };
            botCol = (Color){ 26, 32, 48, 255 };
            break;
    }

    DrawRectangleGradientV(0, 0, w, h, topCol, botCol);

    // Decorative subtle battlefield floor grid
    Color gridColor = (Color){ 255, 255, 255, 10 };
    for (int y = 780; y < h; y += 50) {
        DrawLine(0, y, w, y, gridColor);
    }
}

void WeatherRenderer::DrawWeatherBadge(Rectangle rec, WeatherType weather, const char* prefix, bool isActive) {
    Color wColor = ToRaylibColor(WeatherSystem::GetWeatherColor(weather));
    Color bg = isActive ? (Color){ 32, 40, 58, 255 } : (Color){ 24, 28, 40, 220 };
    Color border = isActive ? wColor : (Color){ 65, 75, 95, 200 };

    DrawRectangleRounded(rec, 0.12f, 8, bg);
    DrawRectangleRoundedLinesEx(rec, 0.12f, 8, isActive ? 3.0f : 2.0f, border);

    if (isActive) {
        // Active Weather Highlight Badge
        std::string title = std::string(prefix) + " " + WeatherSystem::GetWeatherIcon(weather);
        DrawText(title.c_str(), (int)rec.x + 20, (int)rec.y + 14, 26, wColor);
        DrawText(WeatherSystem::GetWeatherShortDesc(weather), (int)rec.x + 20, (int)rec.y + 50, 18, (Color){ 215, 225, 240, 255 });
    } else {
        // Forecast Queue Badge
        std::string title = std::string(prefix) + ": " + WeatherSystem::GetWeatherName(weather);
        DrawText(title.c_str(), (int)rec.x + 16, (int)rec.y + 14, 19, wColor);
        DrawText(WeatherSystem::GetWeatherShortDesc(weather), (int)rec.x + 16, (int)rec.y + 48, 15, (Color){ 165, 175, 195, 255 });
    }
}

void WeatherRenderer::DrawForecastPanel(const WeatherSystem& weatherSystem) {
    int w = ScreenConfig::VIRTUAL_WIDTH;

    // Header Background Bar
    Rectangle barRec = { GameConstants::TOP_BAR_X, GameConstants::TOP_BAR_Y, GameConstants::TOP_BAR_W, GameConstants::TOP_BAR_H };
    DrawRectangleRounded(barRec, 0.08f, 8, (Color){ 20, 25, 38, 235 });
    DrawRectangleRoundedLinesEx(barRec, 0.08f, 8, 2.5f, (Color){ 70, 80, 105, 255 });

    // Section Title
    DrawText("WEATHER FORECAST", 65, 38, 20, (Color){ 160, 175, 200, 255 });
    DrawText("[1-3 Turns Ahead Queue]", 65, 68, 15, (Color){ 120, 130, 150, 255 });

    // 1. Active Weather Card
    WeatherType current = weatherSystem.GetCurrentWeather();
    Rectangle activeRec = { 380.0f, 30.0f, 480.0f, 90.0f };
    DrawWeatherBadge(activeRec, current, "ACTIVE:", true);

    // 2. Next 3 Turns Forecast Queue Cards
    const auto& queue = weatherSystem.GetForecastQueue();
    float startX = 890.0f;
    for (size_t i = 0; i < queue.size() && i < 3; ++i) {
        WeatherType nextW = queue[i];
        Rectangle nextRec = { startX + (float)i * 320.0f, 30.0f, 300.0f, 90.0f };
        std::string prefix = "+" + std::to_string(i + 1) + " Turn";
        DrawWeatherBadge(nextRec, nextW, prefix.c_str(), false);
    }

    // Top Right Utility Navigation Buttons
    Rectangle helpRec = { (float)w - 680.0f, 35.0f, 200.0f, 80.0f };
    DrawRectangleRounded(helpRec, 0.12f, 8, (Color){ 41, 128, 185, 255 });
    DrawRectangleRoundedLinesEx(helpRec, 0.12f, 8, 2.0f, (Color){ 52, 152, 219, 255 });
    DrawText("Guide [H]", (int)helpRec.x + 36, (int)helpRec.y + 26, 24, WHITE);

    Rectangle optRec = { (float)w - 460.0f, 35.0f, 210.0f, 80.0f };
    DrawRectangleRounded(optRec, 0.12f, 8, (Color){ 39, 174, 96, 255 });
    DrawRectangleRoundedLinesEx(optRec, 0.12f, 8, 2.0f, (Color){ 46, 204, 113, 255 });
    DrawText("⚙️ Options [O]", (int)optRec.x + 22, (int)optRec.y + 26, 24, WHITE);

    Rectangle fsRec = { (float)w - 230.0f, 35.0f, 190.0f, 80.0f };
    bool isFs = IsWindowFullscreen();
    std::string fsText = isFs ? "Window" : "Full [F11]";
    DrawRectangleRounded(fsRec, 0.12f, 8, (Color){ 108, 92, 231, 255 });
    DrawRectangleRoundedLinesEx(fsRec, 0.12f, 8, 2.0f, (Color){ 155, 89, 182, 255 });
    int fsW = MeasureText(fsText.c_str(), 24);
    DrawText(fsText.c_str(), (int)(fsRec.x + (fsRec.width - fsW) * 0.5f), (int)fsRec.y + 26, 24, WHITE);
}
