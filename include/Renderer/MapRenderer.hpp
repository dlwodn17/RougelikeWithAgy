#pragma once

#include "Systems/MapSystem.hpp"
#include "Entities/Player.hpp"
#include "Entities/Skill.hpp"
#include "Systems/WeatherSystem.hpp"
#include "Core/Constants.hpp"

// ============================================================================
// Map Renderer (Slay the Spire Branching Graph, Erosion Fog, Rest/Shrine Overlays)
// ============================================================================
class MapRenderer {
public:
    static void DrawMapScreen(const MapSystem& mapSystem, const Player& player, const WeatherSystem& weatherSystem);
    static void DrawRestScreen(const Player& player, const SkillSystem& skillSystem);
    static void DrawShrineScreen(const WeatherSystem& weatherSystem);

private:
    static void DrawPathLine(Vector2 start, Vector2 end, bool isEroded, bool isAvailable, bool isVisited);
    static void DrawNode(const MapNode& node, bool isHovered);
    static void DrawNodeTooltip(const MapNode& node, Vector2 mousePos);
};
