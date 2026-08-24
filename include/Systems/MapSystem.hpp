#pragma once

#include "Core/MapTypes.hpp"
#include "Entities/Player.hpp"
#include "Entities/Skill.hpp"
#include "Systems/WeatherSystem.hpp"
#include <vector>
#include <random>
#include <memory>

// ============================================================================
// Map System (Procedural Branching Graph & Map Erosion Manager)
// ============================================================================
class MapSystem {
public:
    static constexpr int TOTAL_LAYERS = 8;

private:
    std::vector<MapNode> nodes;
    std::vector<std::vector<int>> layerNodeIds;
    int currentNodeId;
    int currentLayer;
    int erosionSurgeCount;
    std::mt19937 rng;

public:
    MapSystem();

    void GenerateNewMap();
    void Reset();

    // Navigation & Node Transitions
    bool CanMoveToNode(int targetNodeId) const;
    bool MoveToNode(int targetNodeId);

    // Queries
    int GetCurrentNodeId() const { return currentNodeId; }
    int GetCurrentLayer() const { return currentLayer; }
    const MapNode* GetCurrentNode() const;
    const MapNode* GetNode(int id) const;
    const std::vector<MapNode>& GetAllNodes() const { return nodes; }
    const std::vector<std::vector<int>>& GetLayerNodeIds() const { return layerNodeIds; }
    int GetErosionSurgeCount() const { return erosionSurgeCount; }

    // Rest Site & Weather Shrine Actions
    void PerformRestHeal(Player& player);
    void PerformRestCleanse(SkillSystem& skillSystem, int skillIndex);
    void PerformShrineWeatherSet(WeatherSystem& weatherSystem, WeatherType desiredWeather);

    // Update animations (e.g. node pulse)
    void Update(float dt);

private:
    void GenerateLayerNodes();
    void ConnectLayerEdges();
    void CalculateNodePositions();
    void ApplyErosionRules();
};
