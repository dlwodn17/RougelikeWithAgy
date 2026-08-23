#pragma once

#include "Entities/Entity.hpp"
#include "Entities/Player.hpp"

class Enemy : public Entity {
private:
    std::string enemyType;
    int tier;
    Intent currentIntent;
    int patternIndex;
    Color color;

public:
    Enemy(const std::string& name = "Enemy", const std::string& type = "Slime", int maxHp = 50,
          Color enemyColor = (Color){ 52, 152, 219, 255 }, int enemyTier = 1);

    void DecideIntent(int turn, const Player& player, WeatherType currentWeather);
    void AdvancePattern();

    const Intent& GetIntent() const { return currentIntent; }
    Color GetColor() const { return color; }
    const std::string& GetEnemyType() const { return enemyType; }
    int GetTier() const { return tier; }
};
