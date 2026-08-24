#include "Systems/MapSystem.hpp"
#include <chrono>
#include <cmath>
#include <algorithm>

MapSystem::MapSystem()
    : currentNodeId(-1), currentLayer(-1), erosionSurgeCount(0),
      rng(static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())) {
    GenerateNewMap();
}

void MapSystem::GenerateNewMap() {
    nodes.clear();
    layerNodeIds.clear();
    layerNodeIds.resize(TOTAL_LAYERS);
    currentNodeId = -1;
    currentLayer = -1;
    erosionSurgeCount = 0;

    GenerateLayerNodes();
    ConnectLayerEdges();
    CalculateNodePositions();

    // Mark Layer 0 nodes as initially available for starting
    for (int nodeId : layerNodeIds[0]) {
        nodes[nodeId].isAvailable = true;
    }
}

void MapSystem::Reset() {
    GenerateNewMap();
}

void MapSystem::GenerateLayerNodes() {
    int nextId = 0;

    // Layer Node Count Config
    // Layer 0: 3, Layer 1: 3, Layer 2: 4, Layer 3: 3, Layer 4: 4, Layer 5: 3, Layer 6: 2, Layer 7: 1 (Boss)
    const int layerCounts[TOTAL_LAYERS] = { 3, 3, 4, 3, 4, 3, 2, 1 };

    for (int l = 0; l < TOTAL_LAYERS; ++l) {
        int count = layerCounts[l];
        for (int i = 0; i < count; ++i) {
            NodeType type = NodeType::COMBAT;

            if (l == 0) {
                type = NodeType::COMBAT;
            } else if (l == 1) {
                type = (i == 1) ? NodeType::WEATHER_SHRINE : NodeType::COMBAT;
            } else if (l == 2) {
                if (i == 0) type = NodeType::COMBAT;
                else if (i == 1) type = NodeType::REST_SITE;
                else if (i == 2) type = NodeType::ELITE;
                else type = NodeType::COMBAT;
            } else if (l == 3) {
                if (i == 0) type = NodeType::ELITE;
                else if (i == 1) type = NodeType::WEATHER_SHRINE;
                else type = NodeType::COMBAT;
            } else if (l == 4) {
                if (i == 0) type = NodeType::COMBAT;
                else if (i == 1) type = NodeType::REST_SITE;
                else if (i == 2) type = NodeType::ELITE;
                else type = NodeType::COMBAT;
            } else if (l == 5) {
                if (i == 0) type = NodeType::ELITE;
                else if (i == 1) type = NodeType::WEATHER_SHRINE;
                else type = NodeType::ELITE;
            } else if (l == 6) {
                type = NodeType::REST_SITE; // Pre-Boss Rest Sanctuary
            } else if (l == 7) {
                type = NodeType::BOSS; // Final Elemental Archon
            }

            MapNode node(nextId, l, i, type, 0.0f, 0.0f);
            if (type == NodeType::ELITE || type == NodeType::BOSS) {
                node.erosionRisk = 1;
            }
            nodes.push_back(node);
            layerNodeIds[l].push_back(nextId);
            nextId++;
        }
    }
}

void MapSystem::CalculateNodePositions() {
    float startX = 320.0f;
    float endX = 2260.0f;
    float layerSpacingX = (endX - startX) / static_cast<float>(TOTAL_LAYERS - 1);

    float minY = 340.0f;
    float maxY = 1140.0f;

    for (int l = 0; l < TOTAL_LAYERS; ++l) {
        float x = startX + static_cast<float>(l) * layerSpacingX;
        const auto& layerIds = layerNodeIds[l];
        int count = static_cast<int>(layerIds.size());

        for (int i = 0; i < count; ++i) {
            float y = (count == 1) ? (minY + maxY) * 0.5f : minY + (static_cast<float>(i) / static_cast<float>(count - 1)) * (maxY - minY);
            // Slight natural jitter for organic map look
            float jitterY = (l > 0 && l < TOTAL_LAYERS - 1 && count > 1) ? ((i % 2 == 0) ? -20.0f : 20.0f) : 0.0f;
            
            nodes[layerIds[i]].posX = x;
            nodes[layerIds[i]].posY = y + jitterY;
            nodes[layerIds[i]].radius = (nodes[layerIds[i]].type == NodeType::BOSS) ? 56.0f : 42.0f;
        }
    }
}

void MapSystem::ConnectLayerEdges() {
    for (int l = 0; l < TOTAL_LAYERS - 1; ++l) {
        const auto& currentLayerIds = layerNodeIds[l];
        const auto& nextLayerIds = layerNodeIds[l + 1];

        // 1. Forward pass: Connect each node in current layer to nearest 1-2 nodes in next layer
        for (int curId : currentLayerIds) {
            MapNode& cur = nodes[curId];

            // Sort next layer nodes by Y-distance
            std::vector<std::pair<float, int>> distList;
            for (int nextId : nextLayerIds) {
                float dist = std::abs(nodes[nextId].posY - cur.posY);
                distList.push_back({ dist, nextId });
            }
            std::sort(distList.begin(), distList.end());

            // Connect nearest node
            int firstNext = distList[0].second;
            cur.nextNodeIds.push_back(firstNext);
            nodes[firstNext].prevNodeIds.push_back(cur.id);

            // Connect second nearest with probability or if branching
            if (distList.size() > 1 && currentLayerIds.size() <= nextLayerIds.size()) {
                int secondNext = distList[1].second;
                cur.nextNodeIds.push_back(secondNext);
                nodes[secondNext].prevNodeIds.push_back(cur.id);
            }
        }

        // 2. Backward pass: Ensure every next-layer node has at least one incoming edge
        for (int nextId : nextLayerIds) {
            MapNode& nextNode = nodes[nextId];
            if (nextNode.prevNodeIds.empty()) {
                // Find closest node in current layer
                int closestPrevId = currentLayerIds[0];
                float minDist = 999999.0f;
                for (int curId : currentLayerIds) {
                    float dist = std::abs(nodes[curId].posY - nextNode.posY);
                    if (dist < minDist) {
                        minDist = dist;
                        closestPrevId = curId;
                    }
                }
                nodes[closestPrevId].nextNodeIds.push_back(nextId);
                nextNode.prevNodeIds.push_back(closestPrevId);
            }
        }

        // Deduplicate edges
        for (int curId : currentLayerIds) {
            auto& nexts = nodes[curId].nextNodeIds;
            std::sort(nexts.begin(), nexts.end());
            nexts.erase(std::unique(nexts.begin(), nexts.end()), nexts.end());
        }
    }
}

bool MapSystem::CanMoveToNode(int targetNodeId) const {
    if (targetNodeId < 0 || targetNodeId >= static_cast<int>(nodes.size())) {
        return false;
    }
    const MapNode& target = nodes[targetNodeId];
    return target.isAvailable && !target.isEroded;
}

bool MapSystem::MoveToNode(int targetNodeId) {
    if (!CanMoveToNode(targetNodeId)) {
        return false;
    }

    if (currentNodeId >= 0 && currentNodeId < static_cast<int>(nodes.size())) {
        nodes[currentNodeId].isCurrent = false;
    }

    currentNodeId = targetNodeId;
    MapNode& curr = nodes[targetNodeId];
    curr.isCurrent = true;
    curr.isVisited = true;
    currentLayer = curr.layer;

    ApplyErosionRules();
    return true;
}

void MapSystem::ApplyErosionRules() {
    erosionSurgeCount = currentLayer;

    for (auto& node : nodes) {
        // 1. Past layers are completely eroded / collapsed
        if (node.layer < currentLayer) {
            node.isEroded = true;
            node.isAvailable = false;
        }
        // 2. Unchosen nodes in current layer are eroded
        else if (node.layer == currentLayer) {
            if (node.id != currentNodeId) {
                node.isEroded = true;
                node.isAvailable = false;
            } else {
                node.isAvailable = false; // Current standing node
            }
        }
        // 3. Next immediate layer: Only nodes connected by outgoing edges are available
        else if (node.layer == currentLayer + 1) {
            const MapNode& curr = nodes[currentNodeId];
            bool isConnected = (std::find(curr.nextNodeIds.begin(), curr.nextNodeIds.end(), node.id) != curr.nextNodeIds.end());
            node.isAvailable = isConnected;
            node.isEroded = false;
        }
        // 4. Distant future layers are not yet available
        else {
            node.isAvailable = false;
            node.isEroded = false;
        }
    }
}

const MapNode* MapSystem::GetCurrentNode() const {
    if (currentNodeId >= 0 && currentNodeId < static_cast<int>(nodes.size())) {
        return &nodes[currentNodeId];
    }
    return nullptr;
}

const MapNode* MapSystem::GetNode(int id) const {
    if (id >= 0 && id < static_cast<int>(nodes.size())) {
        return &nodes[id];
    }
    return nullptr;
}

void MapSystem::PerformRestHeal(Player& player) {
    int healAmount = static_cast<int>(player.GetMaxHp() * 0.35f);
    player.Heal(healAmount);
}

void MapSystem::PerformRestCleanse(SkillSystem& skillSystem, int skillIndex) {
    Skill* s = skillSystem.GetSkill(skillIndex);
    if (s) {
        s->ClearRunes();
        s->ResetCooldown();
    }
}

void MapSystem::PerformShrineWeatherSet(WeatherSystem& weatherSystem, WeatherType desiredWeather) {
    weatherSystem.SetCurrentWeather(desiredWeather);
    // Fill forecast queue with synergistic weather
    auto& queue = weatherSystem.GetForecastQueue();
    for (auto& w : queue) {
        w = desiredWeather;
    }
}

void MapSystem::Update(float dt) {
    for (auto& node : nodes) {
        if (node.isAvailable || node.isCurrent) {
            node.pulseTimer += dt * 3.0f;
            if (node.pulseTimer > 6.28318f) node.pulseTimer -= 6.28318f;
        }
    }
}
