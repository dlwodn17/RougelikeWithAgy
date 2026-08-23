#pragma once

#include "raylib.h"
#include <string>
#include <vector>

class FontManager {
private:
    static Font koreanFont;
    static bool fontLoaded;

public:
    static void Initialize();
    static void Unload();
    static bool IsLoaded() { return fontLoaded; }
    static Font GetFont() { return koreanFont; }

    static void DrawText(const char* text, int posX, int posY, int fontSize, Color color);
    static void DrawText(const std::string& text, int posX, int posY, int fontSize, Color color) {
        DrawText(text.c_str(), posX, posY, fontSize, color);
    }

    static int MeasureText(const char* text, int fontSize);
    static int MeasureText(const std::string& text, int fontSize) {
        return MeasureText(text.c_str(), fontSize);
    }
};
