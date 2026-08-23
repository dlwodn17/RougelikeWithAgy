#pragma once

#include "Common.hpp"

struct Particle {
    Vector2 position;
    Vector2 velocity;
    Color color;
    float size;
    float alpha;
    float life;
    float maxLife;
    bool isSpark;
};

class ParticleSystem {
private:
    std::vector<Particle> particles;
    std::vector<Particle> weatherParticles;
    std::vector<FloatingText> floatingTexts;
    float weatherTimer;
    float lightningFlashTimer;

public:
    ParticleSystem();

    void Update(float dt, WeatherType currentWeather);
    void Draw();

    // Particle Burst Spawners
    void SpawnReactionBurst(Vector2 pos, const std::string& reactionName, Color color);
    void SpawnHitSparks(Vector2 pos, Element elem, int count = 12);
    void SpawnSlashEffect(Vector2 startPos, Vector2 endPos, Color color);
    void TriggerScreenFlash(float duration = 0.15f);

    // Floating Combat Text
    void AddFloatingText(Vector2 pos, const std::string& text, Color color, float fontSize = 22.0f, float lifetime = 1.2f);

    // Clear all
    void ClearAll();

private:
    void UpdateWeatherParticles(float dt, WeatherType currentWeather);
    void DrawWeatherParticles();
};
