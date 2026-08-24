#pragma once

#include "Core/Types.hpp"
#include <vector>
#include <string>

// ============================================================================
// Map Node Types
// ============================================================================
enum class NodeType {
    COMBAT = 0,      // 일반 전투 (Standard Battle)
    ELITE,           // 엘리트 전투 (Elite Encounter - Rare Rune Drops)
    REST_SITE,       // 휴식처 (Rest Site - Heal 30% or Cleanse Runes)
    WEATHER_SHRINE,  // 날씨 제단 (Weather Shrine - Manipulate Weather Queue)
    BOSS             // 최종 보스 (Stage Final Boss - Elemental Archon)
};

// ============================================================================
// Map Node Structure (Slay the Spire Style Procedural Branching Graph)
// ============================================================================
struct MapNode {
    int id = 0;
    int layer = 0;                  // Floor / Depth (0 to 7)
    int indexInLayer = 0;           // Slot index inside this layer (0 to 3)
    NodeType type = NodeType::COMBAT;

    // Virtual Screen Coordinates (2560 x 1440 layout)
    float posX = 0.0f;
    float posY = 0.0f;
    float radius = 42.0f;

    // Directed Graph Edges (Forward connections to next layer)
    std::vector<int> nextNodeIds;
    std::vector<int> prevNodeIds;

    // Dynamic State
    bool isVisited = false;         // Cleared node
    bool isCurrent = false;         // Player currently standing on this node
    bool isEroded = false;          // Sunk into the void / collapsed by erosion
    bool isAvailable = false;       // Valid next destination
    int erosionRisk = 0;            // 0: Normal, 1: Surge (+25% Enemy Stats, +Rare Runes)

    // Visual timers
    float pulseTimer = 0.0f;

    MapNode() = default;

    MapNode(int nId, int nLayer, int nIndex, NodeType nType, float x, float y)
        : id(nId), layer(nLayer), indexInLayer(nIndex), type(nType), posX(x), posY(y) {}
};

inline const char* GetNodeTypeNameKo(NodeType type) {
    switch (type) {
        case NodeType::COMBAT:         return "일반 전투";
        case NodeType::ELITE:          return "엘리트 전투";
        case NodeType::REST_SITE:      return "휴식처";
        case NodeType::WEATHER_SHRINE: return "날씨 제단";
        case NodeType::BOSS:           return "보스 전투";
        default:                       return "미지의 방";
    }
}

inline const char* GetNodeTypeNameEn(NodeType type) {
    switch (type) {
        case NodeType::COMBAT:         return "Combat";
        case NodeType::ELITE:          return "Elite Battle";
        case NodeType::REST_SITE:      return "Rest Site";
        case NodeType::WEATHER_SHRINE: return "Weather Shrine";
        case NodeType::BOSS:           return "Boss Encounter";
        default:                       return "Unknown";
    }
}

inline ColorRGBA GetNodeColorRGBA(NodeType type) {
    switch (type) {
        case NodeType::COMBAT:         return { 189, 195, 199, 255 }; // Light Silver
        case NodeType::ELITE:          return { 231, 76, 60, 255 };   // Crimson Red
        case NodeType::REST_SITE:      return { 46, 204, 113, 255 };  // Emerald Green
        case NodeType::WEATHER_SHRINE: return { 52, 152, 219, 255 };  // Arcane Blue
        case NodeType::BOSS:           return { 241, 196, 15, 255 };  // Royal Gold
        default:                       return { 149, 165, 166, 255 };
    }
}

inline const char* GetNodeSymbol(NodeType type) {
    switch (type) {
        case NodeType::COMBAT:         return "⚔";
        case NodeType::ELITE:          return "☠";
        case NodeType::REST_SITE:      return "⛺";
        case NodeType::WEATHER_SHRINE: return "⚡";
        case NodeType::BOSS:           return "★";
        default:                       return "•";
    }
}
