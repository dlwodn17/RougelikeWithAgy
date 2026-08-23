#pragma once

#include "Core/Constants.hpp"
#include "Systems/CombatSystem.hpp"
#include "Renderer/ParticleSystem.hpp"
#include "Renderer/CombatRenderer.hpp"

class UIRenderer {
private:
    int virtualWidth;
    int virtualHeight;

public:
    UIRenderer(int virtWidth = GameConstants::VIRTUAL_WIDTH, int virtHeight = GameConstants::VIRTUAL_HEIGHT);

    void Initialize();
    void Render(const CombatSystem& combat, const ParticleSystem& particles, GameScene scene,
                int selectedSettingIdx = 0, bool showGuide = false, bool showSettings = false) const;

    void DrawTitleScreen() const;
    void DrawVictoryOverlay(const CombatSystem& combat) const;
    void DrawGameOverOverlay(const CombatSystem& combat) const;
    void DrawSettingsOverlay(int selectedIdx) const;
    void DrawGuideOverlay() const;

    int GetVirtualWidth() const { return virtualWidth; }
    int GetVirtualHeight() const { return virtualHeight; }
};
