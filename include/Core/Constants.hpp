#pragma once

#include "Core/Types.hpp"
#include "raylib.h"

inline Color ToRaylibColor(ColorRGBA c) {
    return (Color){ c.r, c.g, c.b, c.a };
}

inline Vector2 ToRaylibVector2(Vec2 v) {
    return (Vector2){ v.x, v.y };
}


// ==========================================
// Virtual High-Res (2560x1440) Layout Metrics
// ==========================================

namespace GameConstants {
    constexpr int VIRTUAL_WIDTH = 2560;
    constexpr int VIRTUAL_HEIGHT = 1440;

    constexpr float TOP_BAR_X = 40.0f;
    constexpr float TOP_BAR_Y = 20.0f;
    constexpr float TOP_BAR_W = 2480.0f;
    constexpr float TOP_BAR_H = 110.0f;

    constexpr float PLAYER_CARD_X = 60.0f;
    constexpr float PLAYER_CARD_Y = 160.0f;
    constexpr float PLAYER_CARD_W = 620.0f;
    constexpr float PLAYER_CARD_H = 620.0f;

    constexpr float ENEMY_START_X = 720.0f;
    constexpr float ENEMY_CARD_Y = 160.0f;
    constexpr float ENEMY_CARD_H = 620.0f;

    constexpr float STANCE_PANEL_X = 60.0f;
    constexpr float STANCE_PANEL_Y = 810.0f;
    constexpr float STANCE_PANEL_W = 620.0f;
    constexpr float STANCE_PANEL_H = 260.0f;

    constexpr float SKILL_TRAY_X = 720.0f;
    constexpr float SKILL_TRAY_Y = 810.0f;
    constexpr float SKILL_CARD_W = 330.0f;
    constexpr float SKILL_CARD_H = 260.0f;

    constexpr float EXECUTE_BTN_X = 2160.0f;
    constexpr float EXECUTE_BTN_Y = 810.0f;
    constexpr float EXECUTE_BTN_W = 330.0f;
    constexpr float EXECUTE_BTN_H = 260.0f;

    constexpr float LOG_PANEL_X = 60.0f;
    constexpr float LOG_PANEL_Y = 1100.0f;
    constexpr float LOG_PANEL_W = 2440.0f;
    constexpr float LOG_PANEL_H = 290.0f;
}

// ==========================================
// Centralized Font Size Theme (QHD 2560x1440)
// ==========================================
namespace FontSize {
    constexpr int TITLE_LARGE   = 54; // 메인 타이틀
    constexpr int TITLE_MEDIUM  = 46; // 승리 / 패배 오버레이 헤더
    constexpr int MODAL_HEADER  = 34; // 설정 / 도감 창 대형 헤더
    constexpr int CARD_TITLE    = 26; // 영웅 / 적 이름
    constexpr int BUTTON_LARGE  = 24; // 원정 시작, 턴 실행 버튼
    constexpr int BUTTON_MEDIUM = 20; // 탭 / 태세 / 옵션 버튼
    constexpr int BODY_REGULAR  = 20; // 인텐트, 전투 행동 로그
    constexpr int BODY_SMALL    = 18; // 상태이상 배지, 스킬 속성
    constexpr int CAPTION       = 15; // 스킬/날씨 보조 설명
    constexpr int FLOATING_TEXT = 24; // 대미지 / 반응 플로팅 텍스트
}

// ==========================================
// Display & Multi-Resolution Options
// ==========================================

struct ResolutionOption {
    int width;
    int height;
    const char* label;
    const char* tag;
};

class DisplaySettings {
private:
    static inline int currentResolutionIndex = 2; // Default to 1920x1080 (FHD), switchable to QHD

public:
    static const std::vector<ResolutionOption>& GetResolutions() {
        static const std::vector<ResolutionOption> resList = {
            { 1280, 720,  "1280 x 720",  "HD (720p)" },
            { 1600, 900,  "1600 x 900",  "HD+ (900p)" },
            { 1920, 1080, "1920 x 1080", "FHD (1080p)" },
            { 2560, 1440, "2560 x 1440", "QHD (1440p) ★" }
        };
        return resList;
    }

    static void AutoDetectDefaultResolution() {
        int monitor = GetCurrentMonitor();
        int monW = GetMonitorWidth(monitor);
        int monH = GetMonitorHeight(monitor);

        if (monW >= 2560 && monH >= 1440) {
            currentResolutionIndex = 3; // QHD
        } else if (monW >= 1920 && monH >= 1080) {
            currentResolutionIndex = 2; // FHD
        } else if (monW >= 1600 && monH >= 900) {
            currentResolutionIndex = 1; // HD+
        } else {
            currentResolutionIndex = 0; // HD
        }
    }

    static int GetCurrentResolutionIndex() {
        return currentResolutionIndex;
    }

    static void SetResolutionIndex(int index) {
        const auto& list = GetResolutions();
        if (index >= 0 && index < static_cast<int>(list.size())) {
            currentResolutionIndex = index;
            if (!IsWindowFullscreen()) {
                SetWindowSize(list[index].width, list[index].height);
                int monitor = GetCurrentMonitor();
                int monW = GetMonitorWidth(monitor);
                int monH = GetMonitorHeight(monitor);
                SetWindowPosition((monW - list[index].width) / 2, (monH - list[index].height) / 2);
            }
        }
    }

    static void CycleResolution(int delta) {
        const auto& list = GetResolutions();
        int count = static_cast<int>(list.size());
        int newIdx = (currentResolutionIndex + delta) % count;
        if (newIdx < 0) newIdx += count;
        SetResolutionIndex(newIdx);
    }

    static void ToggleFullscreenMode() {
        ::ToggleFullscreen();
        if (!IsWindowFullscreen()) {
            const auto& list = GetResolutions();
            SetWindowSize(list[currentResolutionIndex].width, list[currentResolutionIndex].height);
            int monitor = GetCurrentMonitor();
            int monW = GetMonitorWidth(monitor);
            int monH = GetMonitorHeight(monitor);
            SetWindowPosition((monW - list[currentResolutionIndex].width) / 2, (monH - list[currentResolutionIndex].height) / 2);
        }
    }

    static bool IsFullscreenMode() {
        return IsWindowFullscreen();
    }
};

struct ScreenConfig {
    static constexpr int VIRTUAL_WIDTH = GameConstants::VIRTUAL_WIDTH;
    static constexpr int VIRTUAL_HEIGHT = GameConstants::VIRTUAL_HEIGHT;

    static Vector2 GetVirtualMousePosition() {
        Vector2 mouse = ::GetMousePosition();
        int renderW = ::GetRenderWidth();
        int renderH = ::GetRenderHeight();
        int screenW = ::GetScreenWidth();
        int screenH = ::GetScreenHeight();

        if (screenW <= 0 || screenH <= 0) return mouse;

        if (renderW > 0 && screenW > 0 && renderW != screenW) {
            float dpiScaleX = (float)renderW / (float)screenW;
            float dpiScaleY = (float)renderH / (float)screenH;
            mouse.x *= dpiScaleX;
            mouse.y *= dpiScaleY;
            screenW = renderW;
            screenH = renderH;
        }

        float scale = std::min((float)screenW / (float)VIRTUAL_WIDTH, (float)screenH / (float)VIRTUAL_HEIGHT);
        if (scale <= 0.0001f) scale = 1.0f;

        float offsetX = ((float)screenW - ((float)VIRTUAL_WIDTH * scale)) * 0.5f;
        float offsetY = ((float)screenH - ((float)VIRTUAL_HEIGHT * scale)) * 0.5f;
        return (Vector2){ (mouse.x - offsetX) / scale, (mouse.y - offsetY) / scale };
    }

    static void ToggleGameFullscreen() {
        DisplaySettings::ToggleFullscreenMode();
    }
};
