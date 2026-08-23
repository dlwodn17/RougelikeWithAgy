#include "Renderer/WeatherRenderer.hpp"
#include "Renderer/FontManager.hpp"
#include "Core/Localization.hpp"
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

    // Subtle battlefield floor grid
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
        std::string title = std::string(prefix) + " " + Localization::GetWeatherName(weather);
        FontManager::DrawText(title.c_str(), (int)rec.x + 20, (int)rec.y + 14, FontSize::BUTTON_LARGE, wColor);
        FontManager::DrawText(Localization::GetWeatherShortDesc(weather), (int)rec.x + 20, (int)rec.y + 50, 17, (Color){ 215, 225, 240, 255 });
    } else {
        std::string title = std::string(prefix) + ": " + Localization::GetWeatherName(weather);
        FontManager::DrawText(title.c_str(), (int)rec.x + 16, (int)rec.y + 14, FontSize::BUTTON_MEDIUM, wColor);
        FontManager::DrawText(Localization::GetWeatherShortDesc(weather), (int)rec.x + 16, (int)rec.y + 48, FontSize::CAPTION, (Color){ 165, 175, 195, 255 });
    }
}

void WeatherRenderer::DrawForecastPanel(const WeatherSystem& weatherSystem) {
    int w = ScreenConfig::VIRTUAL_WIDTH;

    // Header Background Bar
    Rectangle barRec = { GameConstants::TOP_BAR_X, GameConstants::TOP_BAR_Y, GameConstants::TOP_BAR_W, GameConstants::TOP_BAR_H };
    DrawRectangleRounded(barRec, 0.08f, 8, (Color){ 20, 25, 38, 235 });
    DrawRectangleRoundedLinesEx(barRec, 0.08f, 8, 2.5f, (Color){ 70, 80, 105, 255 });

    // Section Title
    if (Localization::IsKorean()) {
        FontManager::DrawText("날씨 예보 시스템", 60, 36, FontSize::BODY_REGULAR, (Color){ 160, 175, 200, 255 });
        FontManager::DrawText("[향후 3턴 예보 큐]", 60, 66, FontSize::CAPTION, (Color){ 120, 130, 150, 255 });
    } else {
        FontManager::DrawText("WEATHER FORECAST", 60, 36, FontSize::BODY_REGULAR, (Color){ 160, 175, 200, 255 });
        FontManager::DrawText("[1-3 Turns Ahead Queue]", 60, 66, FontSize::CAPTION, (Color){ 120, 130, 150, 255 });
    }

    // 1. Active Weather Card
    WeatherType current = weatherSystem.GetCurrentWeather();
    Rectangle activeRec = { 380.0f, 30.0f, 480.0f, 90.0f };
    DrawWeatherBadge(activeRec, current, Localization::IsKorean() ? "현재 날씨:" : "ACTIVE:", true);

    // 2. Next 3 Turns Forecast Queue Cards
    const auto& queue = weatherSystem.GetForecastQueue();
    float startX = 890.0f;
    for (size_t i = 0; i < queue.size() && i < 3; ++i) {
        WeatherType nextW = queue[i];
        Rectangle nextRec = { startX + (float)i * 320.0f, 30.0f, 300.0f, 90.0f };
        std::string prefix = Localization::IsKorean() ? ("+" + std::to_string(i + 1) + "턴 뒤") : ("+" + std::to_string(i + 1) + " Turn");
        DrawWeatherBadge(nextRec, nextW, prefix.c_str(), false);
    }

    // Top Right Utility Navigation Buttons
    // 1. Language Toggle Button [L]
    Rectangle langRec = { (float)w - 870.0f, 35.0f, 170.0f, 80.0f };
    DrawRectangleRounded(langRec, 0.12f, 8, (Color){ 230, 126, 34, 255 });
    DrawRectangleRoundedLinesEx(langRec, 0.12f, 8, 2.0f, (Color){ 243, 156, 18, 255 });
    std::string langText = Localization::IsKorean() ? "언어: 한국어" : "Lang: ENG";
    int ltw = FontManager::MeasureText(langText.c_str(), FontSize::BUTTON_MEDIUM);
    FontManager::DrawText(langText.c_str(), (int)(langRec.x + (langRec.width - ltw) * 0.5f), (int)langRec.y + 18, FontSize::BUTTON_MEDIUM, WHITE);
    FontManager::DrawText("[ L ]", (int)(langRec.x + (langRec.width - FontManager::MeasureText("[ L ]", 16)) * 0.5f), (int)langRec.y + 48, 16, (Color){ 255, 230, 180, 255 });

    // 2. Guide Button [H]
    Rectangle helpRec = { (float)w - 680.0f, 35.0f, 200.0f, 80.0f };
    DrawRectangleRounded(helpRec, 0.12f, 8, (Color){ 41, 128, 185, 255 });
    DrawRectangleRoundedLinesEx(helpRec, 0.12f, 8, 2.0f, (Color){ 52, 152, 219, 255 });
    std::string helpText = Localization::IsKorean() ? "도움말 [H]" : "Guide [H]";
    int htw = FontManager::MeasureText(helpText.c_str(), FontSize::BUTTON_LARGE);
    FontManager::DrawText(helpText.c_str(), (int)(helpRec.x + (helpRec.width - htw) * 0.5f), (int)helpRec.y + 26, FontSize::BUTTON_LARGE, WHITE);

    // 3. Options Button [O]
    Rectangle optRec = { (float)w - 460.0f, 35.0f, 210.0f, 80.0f };
    DrawRectangleRounded(optRec, 0.12f, 8, (Color){ 39, 174, 96, 255 });
    DrawRectangleRoundedLinesEx(optRec, 0.12f, 8, 2.0f, (Color){ 46, 204, 113, 255 });
    std::string optText = Localization::IsKorean() ? "설정 [O]" : "Options [O]";
    int otw = FontManager::MeasureText(optText.c_str(), FontSize::BUTTON_LARGE);
    FontManager::DrawText(optText.c_str(), (int)(optRec.x + (optRec.width - otw) * 0.5f), (int)optRec.y + 26, FontSize::BUTTON_LARGE, WHITE);

    // 4. Fullscreen Button [F11]
    Rectangle fsRec = { (float)w - 230.0f, 35.0f, 190.0f, 80.0f };
    bool isFs = IsWindowFullscreen();
    std::string fsText = isFs ? (Localization::IsKorean() ? "창모드" : "Window") : (Localization::IsKorean() ? "전체화면" : "Fullscreen");
    DrawRectangleRounded(fsRec, 0.12f, 8, (Color){ 108, 92, 231, 255 });
    DrawRectangleRoundedLinesEx(fsRec, 0.12f, 8, 2.0f, (Color){ 155, 89, 182, 255 });
    int fsW = FontManager::MeasureText(fsText.c_str(), FontSize::BUTTON_MEDIUM + 2);
    FontManager::DrawText(fsText.c_str(), (int)(fsRec.x + (fsRec.width - fsW) * 0.5f), (int)fsRec.y + 16, FontSize::BUTTON_MEDIUM + 2, WHITE);
    FontManager::DrawText("[F11]", (int)(fsRec.x + (fsRec.width - FontManager::MeasureText("[F11]", 16)) * 0.5f), (int)fsRec.y + 46, 16, (Color){ 220, 205, 255, 255 });
}
