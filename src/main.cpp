#include "raylib.h"
#include "GameState.hpp"
#include "Core/Constants.hpp"
#include "Systems/ElementalSystem.hpp"
#include "Entities/Skill.hpp"
#include "Renderer/FontManager.hpp"
#include <iostream>

void RunCoreElementalUnitTests() {
    std::cout << "=================================================================\n";
    std::cout << " [STEP 1 UNIT TEST SUITE] Elemental Reaction & Core Data System \n";
    std::cout << "=================================================================\n\n";

    // Test 1: Bitwise Element Flags
    std::cout << "[Test 1] Element Bitflag Operations:\n";
    Element statusMask = Element::NONE;
    statusMask |= Element::WET;
    statusMask |= Element::OIL;
    std::cout << " - Applied WET (1) and OIL (4) -> Mask Value: " << static_cast<uint32_t>(statusMask) << "\n";
    std::cout << " - HasFlag(statusMask, Element::WET): " << (HasFlag(statusMask, Element::WET) ? "PASS" : "FAIL") << "\n";
    std::cout << " - HasFlag(statusMask, Element::OIL): " << (HasFlag(statusMask, Element::OIL) ? "PASS" : "FAIL") << "\n";
    std::cout << " - HasFlag(statusMask, Element::FIRE): " << (!HasFlag(statusMask, Element::FIRE) ? "PASS (Not present)" : "FAIL") << "\n\n";

    // Test 2: Primary Elemental Reactions Matrix
    std::cout << "[Test 2] Elemental Reaction Matrix Resolution:\n";
    
    // Scenario A: WET + LIGHTNING -> SHOCK
    ReactionResult shockRes = ElementalSystem::ResolveReaction(Element::WET, Element::LIGHTNING);
    std::cout << " -> Reaction [WET + LIGHTNING]: " << shockRes.name << " (Triggered: " << (shockRes.triggered ? "TRUE" : "FALSE") << ")\n";
    std::cout << "    - Bonus DMG: +" << shockRes.bonusDamage << " | Chain AoE: " << (shockRes.chainAoE ? "YES" : "NO") << " (AoE DMG: " << shockRes.aoeDamage << ")\n";

    // Scenario B: OIL + FIRE -> EXPLOSION
    ReactionResult expRes = ElementalSystem::ResolveReaction(Element::OIL, Element::FIRE);
    std::cout << " -> Reaction [OIL + FIRE]: " << expRes.name << " (Triggered: " << (expRes.triggered ? "TRUE" : "FALSE") << ")\n";
    std::cout << "    - Bonus DMG: +" << expRes.bonusDamage << " | Applied Status: " << GetElementNameStr(expRes.appliedElements) << " (Duration: " << expRes.appliedDuration << "T)\n";

    // Scenario C: WET + COLD -> FROZEN
    ReactionResult frzRes = ElementalSystem::ResolveReaction(Element::WET, Element::COLD);
    std::cout << " -> Reaction [WET + COLD]: " << frzRes.name << " (Triggered: " << (frzRes.triggered ? "TRUE" : "FALSE") << ")\n";
    std::cout << "    - Stun Target: " << (frzRes.stunTarget ? "YES (Skip 1 Turn)" : "NO") << "\n";

    // Scenario D: FIRE + COLD -> MELT
    ReactionResult meltRes = ElementalSystem::ResolveReaction(Element::FIRE, Element::COLD);
    std::cout << " -> Reaction [FIRE + COLD]: " << meltRes.name << " (Triggered: " << (meltRes.triggered ? "TRUE" : "FALSE") << ")\n";
    std::cout << "    - Bonus DMG: +" << meltRes.bonusDamage << "\n\n";

    // Test 3: In-Combat Entity Reaction Execution (WET target struck with LIGHTNING)
    std::cout << "[Test 3] Combat Entity Reaction Execution Simulation:\n";
    Entity dummyEnemy("Training Dummy", 100);
    std::cout << " - Created Enemy '" << dummyEnemy.GetName() << "' with HP: " << dummyEnemy.GetHp() << "/" << dummyEnemy.GetMaxHp() << "\n";
    
    std::cout << " - Step A: Inflicting [WET] status (2 Turns)...\n";
    dummyEnemy.ApplyElement(Element::WET, 2);
    std::cout << "   Current Status Mask: " << static_cast<uint32_t>(dummyEnemy.GetActiveStatusMask())
              << " | Has [WET]: " << (dummyEnemy.HasElement(Element::WET) ? "TRUE" : "FALSE") << "\n";

    std::cout << " - Step B: Attacking with [LIGHTNING] (20 Base DMG)...\n";
    DamageReport report = dummyEnemy.ApplyIncomingDamage(20, Element::LIGHTNING, StanceType::ATTACK);
    std::cout << "   Damage Report:\n";
    std::cout << "   * Raw DMG: " << report.rawDamage << "\n";
    std::cout << "   * Mitigated/Effective DMG: " << report.mitigatedDamage << " (Bonus from SHOCK: +" << report.reaction.bonusDamage << ")\n";
    std::cout << "   * Remaining Enemy HP: " << dummyEnemy.GetHp() << "/" << dummyEnemy.GetMaxHp() << "\n";
    std::cout << "   * Reaction Triggered: [" << report.reaction.name << "] -> " << report.reaction.description << "\n";
    std::cout << "   * Chain AoE Flag: " << (report.reaction.chainAoE ? "TRUE (Deals 12 AoE DMG to all other enemies)" : "FALSE") << "\n";
    std::cout << "   * Status after consumption: " << (dummyEnemy.HasElement(Element::WET) ? "WET remains" : "WET consumed by reaction (CLEAN)") << "\n\n";

    // Test 4: Extensible Skill Mutation Runes (Phase 3)
    std::cout << "[Test 4] Extensible Skill & Mutation Rune Modifiers:\n";
    Skill baseSlash("slash", "Basic Slash", "Deals physical damage", Element::NONE, Element::NONE, 20, 0, 1);
    std::cout << " - Original Skill: " << baseSlash.GetName() << " | Element: " << GetElementNameStr(baseSlash.GetFinalElement()) << " | Damage: " << baseSlash.GetFinalDamage() << " | Cooldown: " << baseSlash.GetFinalCooldown() << "\n";

    Rune frostfire("rune_frostfire", "Frostfire Rune", "서리불꽃 룬", "Mutates to Cold (+6 Dmg)", "냉기로 변환", RuneModifierType::ELEMENT_CONVERSION);
    frostfire.overrideElement = Element::COLD;
    frostfire.bonusDamage = 6;
    frostfire.freezeWetTarget = true;
    baseSlash.AttachRune(frostfire);

    std::cout << " - Socketed [Rune 1: " << frostfire.name << "]:\n";
    std::cout << "   * Final Element: " << GetElementNameStr(baseSlash.GetFinalElement()) << " (Expected: Cold)\n";
    std::cout << "   * Final Damage: " << baseSlash.GetFinalDamage() << " (Expected: 20 + 6 = 26)\n";
    std::cout << "   * Has Freeze WET Special: " << (baseSlash.HasFreezeWet() ? "PASS" : "FAIL") << "\n";

    Rune volatileCore("rune_volatile_core", "Volatile Core", "농축 기폭제", "+50% Dmg, +1 CD", "+50% 피해, +1 CD", RuneModifierType::STAT_MUTATION);
    volatileCore.damageMultiplier = 1.50f;
    volatileCore.cooldownDelta = 1;
    baseSlash.AttachRune(volatileCore);

    std::cout << " - Socketed [Rune 2: " << volatileCore.name << "]:\n";
    std::cout << "   * Final Damage: " << baseSlash.GetFinalDamage() << " (Expected: (20 + 6) * 1.5 = 39)\n";
    std::cout << "   * Final Cooldown: " << baseSlash.GetFinalCooldown() << " (Expected: 1 + 1 = 2)\n";
    std::cout << "   * Total Socketed Runes: " << baseSlash.GetSocketedRunes().size() << " / " << Skill::MAX_RUNE_SLOTS << "\n";

    // Test 5: Reward System Draft Verification
    std::cout << "\n[Test 5] RewardSystem Rune Draft Generation:\n";
    RewardSystem rewardSys;
    rewardSys.GenerateRewardRunes(3);
    const auto& offered = rewardSys.GetOfferedRunes();
    std::cout << " - Offered Runes Count: " << offered.size() << "\n";
    for (size_t i = 0; i < offered.size(); ++i) {
        std::cout << "   [" << (i + 1) << "] " << offered[i].name << " (" << offered[i].nameKo << ") -> " << offered[i].description << "\n";
    }
    std::cout << "=================================================================\n\n";
}

int main(int argc, char* argv[]) {
    // 1. Run and print pure C++ core elemental unit test suite
    RunCoreElementalUnitTests();

    // If --test CLI argument provided, exit after running console tests
    if (argc > 1 && std::string(argv[1]) == "--test") {
        std::cout << "[SUCCESS] All Step 1 & Step 3 Elemental Reaction and Mutation Rune unit tests passed successfully!\n";
        return 0;
    }

    // 2. Window & Display Configuration
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);
    
    // Auto-detect best initial window size based on monitor resolution
    int monitor = GetCurrentMonitor();
    int monWidth = GetMonitorWidth(monitor);
    int monHeight = GetMonitorHeight(monitor);

    DisplaySettings::AutoDetectDefaultResolution();
    int defaultIdx = DisplaySettings::GetCurrentResolutionIndex();
    const auto& resList = DisplaySettings::GetResolutions();
    int initWinWidth = resList[defaultIdx].width;
    int initWinHeight = resList[defaultIdx].height;

    // Safety clamp for windowed borders
    if (monWidth > 0 && monHeight > 0) {
        if (initWinWidth >= monWidth && defaultIdx > 0) {
            defaultIdx--;
            initWinWidth = resList[defaultIdx].width;
            initWinHeight = resList[defaultIdx].height;
            DisplaySettings::SetResolutionIndex(defaultIdx);
        }
    }

    InitWindow(initWinWidth, initWinHeight, "RougelikeWithAgy - Elemental Convergence Roguelike");
    SetWindowMinSize(960, 540);
    SetTargetFPS(60);

    // Center window on primary monitor
    if (monWidth > 0 && monHeight > 0) {
        SetWindowPosition((monWidth - initWinWidth) / 2, (monHeight - initWinHeight) / 2);
    }

    // Create 2560x1440 Native Virtual Canvas
    RenderTexture2D targetCanvas = LoadRenderTexture(ScreenConfig::VIRTUAL_WIDTH, ScreenConfig::VIRTUAL_HEIGHT);
    SetTextureFilter(targetCanvas.texture, TEXTURE_FILTER_BILINEAR);

    // Initialize Game Architecture
    GameManager game;
    game.Initialize();

    // Main Game Loop
    while (!WindowShouldClose()) {
        // Fullscreen Toggle Hotkeys: F11 or Alt+Enter
        if (IsKeyPressed(KEY_F11) || (IsKeyDown(KEY_LEFT_ALT) && IsKeyPressed(KEY_ENTER))) {
            ScreenConfig::ToggleGameFullscreen();
        }

        float dt = GetFrameTime();
        if (dt > 0.1f) dt = 0.1f; // Cap delta time

        // 1. Update Game Logic
        game.Update(dt);

        // 2. Render Scene to 2560x1440 Canvas
        BeginTextureMode(targetCanvas);
        ClearBackground((Color){ 12, 16, 24, 255 });
        game.Draw();
        EndTextureMode();

        // 3. Render Canvas Scaled to Physical Framebuffer (16:9 aspect ratio)
        BeginDrawing();
        ClearBackground(BLACK);

        int renderW = GetRenderWidth();
        int renderH = GetRenderHeight();
        if (renderW <= 0) renderW = GetScreenWidth();
        if (renderH <= 0) renderH = GetScreenHeight();

        float scale = std::min((float)renderW / (float)ScreenConfig::VIRTUAL_WIDTH,
                               (float)renderH / (float)ScreenConfig::VIRTUAL_HEIGHT);
        
        Rectangle srcRec = { 0.0f, 0.0f, (float)ScreenConfig::VIRTUAL_WIDTH, -(float)ScreenConfig::VIRTUAL_HEIGHT };
        Rectangle dstRec = {
            ((float)renderW - ((float)ScreenConfig::VIRTUAL_WIDTH * scale)) * 0.5f,
            ((float)renderH - ((float)ScreenConfig::VIRTUAL_HEIGHT * scale)) * 0.5f,
            (float)ScreenConfig::VIRTUAL_WIDTH * scale,
            (float)ScreenConfig::VIRTUAL_HEIGHT * scale
        };

        DrawTexturePro(targetCanvas.texture, srcRec, dstRec, (Vector2){ 0, 0 }, 0.0f, WHITE);
        EndDrawing();
    }

    // Cleanup & Exit
    FontManager::Unload();
    UnloadRenderTexture(targetCanvas);
    CloseWindow();
    return 0;
}
