#pragma once

#include "Core/Constants.hpp"

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

struct ParticleFloatingText {
    Vector2 position;
    Vector2 velocity;
    std::string text;
    Color color;
    float lifetime;
    float maxLifetime;
    float fontSize;
    float alpha;
};

class ParticleSystem {
private:
    std::vector<Particle> particles;
    std::vector<Particle> weatherParticles;
    std::vector<ParticleFloatingText> floatingTexts;
    float weatherTimer;
    float lightningFlashTimer;

public:
    ParticleSystem();

    void ClearAll();
    void TriggerScreenFlash(float duration = 0.15f);

    void AddFloatingText(Vector2 pos, const std::string& text, Color color, float fontSize = 20.0f, float lifetime = 1.2f);
    void AddFloatingText(Vec2 pos, const std::string& text, Color color, float fontSize = 20.0f, float lifetime = 1.2f) {
        AddFloatingText((Vector2){ pos.x, pos.y }, text, color, fontSize, lifetime);
    }

    void SpawnReactionBurst(Vector2 pos, const std::string& reactionName, Color color);
    void SpawnReactionBurst(Vec2 pos, const std::string& reactionName, Color color) {
        SpawnReactionBurst((Vector2){ pos.x, pos.y }, reactionName, color);
    }

    void SpawnHitSparks(Vector2 pos, Element elem, int count = 12);
    void SpawnHitSparks(Vec2 pos, Element elem, int count = 12) {
        SpawnHitSparks((Vector2){ pos.x, pos.y }, elem, count);
    }

    void SpawnSlashEffect(Vector2 startPos, Vector2 endPos, Color color);
    void SpawnSlashEffect(Vec2 startPos, Vec2 endPos, Color color) {
        SpawnSlashEffect((Vector2){ startPos.x, startPos.y }, (Vector2){ endPos.x, endPos.y }, color);
    }

    void Update(float dt, WeatherType currentWeather);
    void Draw();

private:
    void UpdateWeatherParticles(float dt, WeatherType currentWeather);
};
