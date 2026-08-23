#include "Entities/Player.hpp"

Player::Player(const std::string& playerName, int maxHealth)
    : Entity(playerName, maxHealth), stance(StanceType::ATTACK) {
}
