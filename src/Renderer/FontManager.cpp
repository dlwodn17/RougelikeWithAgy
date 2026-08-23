#include "Renderer/FontManager.hpp"
#include <iostream>

Font FontManager::koreanFont = {};
bool FontManager::fontLoaded = false;

void FontManager::Initialize() {
    if (fontLoaded) return;

    std::vector<int> codepoints;
    // 1. ASCII 32 to 126
    for (int i = 32; i <= 126; ++i) codepoints.push_back(i);
    // 2. Korean Syllables (0xAC00 to 0xD7A3)
    for (int i = 0xAC00; i <= 0xD7A3; ++i) codepoints.push_back(i);
    // 3. Korean Jamo (0x3131 to 0x318E)
    for (int i = 0x3131; i <= 0x318E; ++i) codepoints.push_back(i);
    // 4. Common Game Symbols
    codepoints.push_back(0x2605); // ★
    codepoints.push_back(0x2606); // ☆
    codepoints.push_back(0x2694); // ⚔
    codepoints.push_back(0x26A1); // ⚡
    codepoints.push_back(0x2714); // ✔
    codepoints.push_back(0x2716); // ✖
    codepoints.push_back(0x25BC); // ▼
    codepoints.push_back(0x25B2); // ▲
    codepoints.push_back(0x25B6); // ▶
    codepoints.push_back(0x25C0); // ◀
    codepoints.push_back(0x2022); // •
    codepoints.push_back(0x2192); // →
    codepoints.push_back(0x2620); // ☠

    const char* fontPaths[] = {
        "C:/Windows/Fonts/malgun.ttf",
        "C:/Windows/Fonts/gulim.ttc",
        "C:/Windows/Fonts/batang.ttc"
    };

    for (const char* path : fontPaths) {
        if (FileExists(path)) {
            koreanFont = LoadFontEx(path, 36, codepoints.data(), static_cast<int>(codepoints.size()));
            if (koreanFont.baseSize > 0) {
                SetTextureFilter(koreanFont.texture, TEXTURE_FILTER_BILINEAR);
                fontLoaded = true;
                break;
            }
        }
    }

    if (!fontLoaded) {
        koreanFont = GetFontDefault();
    }
}

void FontManager::Unload() {
    if (fontLoaded && koreanFont.baseSize > 0) {
        UnloadFont(koreanFont);
        fontLoaded = false;
    }
}

void FontManager::DrawText(const char* text, int posX, int posY, int fontSize, Color color) {
    if (fontLoaded) {
        DrawTextEx(koreanFont, text, (Vector2){ static_cast<float>(posX), static_cast<float>(posY) }, static_cast<float>(fontSize), 1.0f, color);
    } else {
        ::DrawText(text, posX, posY, fontSize, color);
    }
}

int FontManager::MeasureText(const char* text, int fontSize) {
    if (fontLoaded) {
        return static_cast<int>(MeasureTextEx(koreanFont, text, static_cast<float>(fontSize), 1.0f).x);
    } else {
        return ::MeasureText(text, fontSize);
    }
}
