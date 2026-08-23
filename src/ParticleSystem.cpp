#include "ParticleSystem.hpp"
#include <random>

static float RandomFloat(float min, float max) {
    static std::mt19937 rng(1337);
    std::uniform_real_distribution<float> dist(min, max);
    return dist(rng);
}

ParticleSystem::ParticleSystem() : weatherTimer(0.0f), lightningFlashTimer(0.0f) {
}

void ParticleSystem::ClearAll() {
    particles.clear();
    weatherParticles.clear();
    floatingTexts.clear();
}

void ParticleSystem::TriggerScreenFlash(float duration) {
    lightningFlashTimer = duration;
}

void ParticleSystem::AddFloatingText(Vector2 pos, const std::string& text, Color color, float fontSize, float lifetime) {
    FloatingText ft;
    ft.position = (Vector2){ pos.x + RandomFloat(-15.0f, 15.0f), pos.y - 10.0f };
    ft.velocity = (Vector2){ RandomFloat(-15.0f, 15.0f), RandomFloat(-55.0f, -40.0f) };
    ft.text = text;
    ft.color = color;
    ft.lifetime = lifetime;
    ft.maxLifetime = lifetime;
    ft.fontSize = fontSize;
    ft.alpha = 1.0f;
    floatingTexts.push_back(ft);
}

void ParticleSystem::SpawnReactionBurst(Vector2 pos, const std::string& reactionName, Color color) {
    (void)reactionName;
    for (int i = 0; i < 30; ++i) {
        Particle p;
        p.position = pos;
        float angle = RandomFloat(0.0f, 6.28318f);
        float speed = RandomFloat(60.0f, 220.0f);
        p.velocity = (Vector2){ cosf(angle) * speed, sinf(angle) * speed };
        p.color = color;
        p.size = RandomFloat(4.0f, 9.0f);
        p.alpha = 1.0f;
        p.life = RandomFloat(0.5f, 0.9f);
        p.maxLife = p.life;
        p.isSpark = true;
        particles.push_back(p);
    }
}

void ParticleSystem::SpawnHitSparks(Vector2 pos, Element elem, int count) {
    Color col = GetElementBaseColor(elem);
    for (int i = 0; i < count; ++i) {
        Particle p;
        p.position = pos;
        float angle = RandomFloat(0.0f, 6.28318f);
        float speed = RandomFloat(40.0f, 160.0f);
        p.velocity = (Vector2){ cosf(angle) * speed, sinf(angle) * speed };
        p.color = col;
        p.size = RandomFloat(3.0f, 6.0f);
        p.alpha = 1.0f;
        p.life = RandomFloat(0.3f, 0.6f);
        p.maxLife = p.life;
        p.isSpark = false;
        particles.push_back(p);
    }
}

void ParticleSystem::SpawnSlashEffect(Vector2 startPos, Vector2 endPos, Color color) {
    int segments = 10;
    for (int i = 0; i <= segments; ++i) {
        float t = (float)i / (float)segments;
        Vector2 pt = {
            startPos.x + (endPos.x - startPos.x) * t,
            startPos.y + (endPos.y - startPos.y) * t
        };
        Particle p;
        p.position = pt;
        p.velocity = (Vector2){ RandomFloat(-20.0f, 20.0f), RandomFloat(-20.0f, 20.0f) };
        p.color = color;
        p.size = RandomFloat(4.0f, 7.0f);
        p.alpha = 1.0f;
        p.life = 0.35f;
        p.maxLife = 0.35f;
        p.isSpark = true;
        particles.push_back(p);
    }
}

void ParticleSystem::UpdateWeatherParticles(float dt, WeatherType currentWeather) {
    weatherTimer += dt;

    // Spawn ambient weather particles
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    if (currentWeather == WeatherType::RAIN || currentWeather == WeatherType::THUNDERSTORM) {
        for (int i = 0; i < 4; ++i) {
            Particle p;
            p.position = (Vector2){ RandomFloat(0.0f, (float)screenWidth), -10.0f };
            p.velocity = (Vector2){ -40.0f, RandomFloat(450.0f, 650.0f) };
            p.color = (Color){ 100, 180, 255, 180 };
            p.size = RandomFloat(2.0f, 3.5f);
            p.alpha = 0.7f;
            p.life = 2.0f;
            p.maxLife = 2.0f;
            p.isSpark = false;
            weatherParticles.push_back(p);
        }
        if (currentWeather == WeatherType::THUNDERSTORM && RandomFloat(0.0f, 100.0f) < 1.2f) {
            TriggerScreenFlash(0.12f);
        }
    } else if (currentWeather == WeatherType::HEATWAVE) {
        for (int i = 0; i < 2; ++i) {
            Particle p;
            p.position = (Vector2){ RandomFloat(0.0f, (float)screenWidth), (float)screenHeight + 10.0f };
            p.velocity = (Vector2){ RandomFloat(-20.0f, 20.0f), RandomFloat(-80.0f, -140.0f) };
            p.color = (Color){ 255, 120, 40, 200 };
            p.size = RandomFloat(3.0f, 6.0f);
            p.alpha = 0.8f;
            p.life = 2.5f;
            p.maxLife = 2.5f;
            p.isSpark = true;
            weatherParticles.push_back(p);
        }
    } else if (currentWeather == WeatherType::BLIZZARD) {
        for (int i = 0; i < 5; ++i) {
            Particle p;
            p.position = (Vector2){ RandomFloat(-50.0f, (float)screenWidth), -10.0f };
            p.velocity = (Vector2){ RandomFloat(150.0f, 280.0f), RandomFloat(180.0f, 300.0f) };
            p.color = (Color){ 220, 245, 255, 220 };
            p.size = RandomFloat(2.5f, 5.0f);
            p.alpha = 0.8f;
            p.life = 2.0f;
            p.maxLife = 2.0f;
            p.isSpark = false;
            weatherParticles.push_back(p);
        }
    } else if (currentWeather == WeatherType::GALE_WINDS) {
        if (RandomFloat(0.0f, 10.0f) < 3.0f) {
            Particle p;
            p.position = (Vector2){ -20.0f, RandomFloat(50.0f, (float)screenHeight - 100.0f) };
            p.velocity = (Vector2){ RandomFloat(500.0f, 750.0f), RandomFloat(-15.0f, 15.0f) };
            p.color = (Color){ 160, 240, 180, 150 };
            p.size = RandomFloat(2.0f, 4.0f);
            p.alpha = 0.6f;
            p.life = 1.8f;
            p.maxLife = 1.8f;
            p.isSpark = false;
            weatherParticles.push_back(p);
        }
    } else if (currentWeather == WeatherType::ACID_RAIN) {
        for (int i = 0; i < 3; ++i) {
            Particle p;
            p.position = (Vector2){ RandomFloat(0.0f, (float)screenWidth), -10.0f };
            p.velocity = (Vector2){ -20.0f, RandomFloat(350.0f, 500.0f) };
            p.color = (Color){ 160, 110, 240, 180 };
            p.size = RandomFloat(2.5f, 4.0f);
            p.alpha = 0.7f;
            p.life = 2.2f;
            p.maxLife = 2.2f;
            p.isSpark = false;
            weatherParticles.push_back(p);
        }
    }

    // Update weather particles
    for (auto it = weatherParticles.begin(); it != weatherParticles.end();) {
        it->position.x += it->velocity.x * dt;
        it->position.y += it->velocity.y * dt;
        it->life -= dt;

        if (it->life <= 0.0f || it->position.y > screenHeight + 20 || it->position.x > screenWidth + 50) {
            it = weatherParticles.erase(it);
        } else {
            ++it;
        }
    }
}

void ParticleSystem::Update(float dt, WeatherType currentWeather) {
    if (lightningFlashTimer > 0.0f) {
        lightningFlashTimer -= dt;
    }

    UpdateWeatherParticles(dt, currentWeather);

    // Update combat burst particles
    for (auto it = particles.begin(); it != particles.end();) {
        it->position.x += it->velocity.x * dt;
        it->position.y += it->velocity.y * dt;
        it->velocity.x *= 0.95f;
        it->velocity.y *= 0.95f;
        it->life -= dt;
        it->alpha = std::max(0.0f, it->life / it->maxLife);

        if (it->life <= 0.0f) {
            it = particles.erase(it);
        } else {
            ++it;
        }
    }

    // Update floating texts
    for (auto it = floatingTexts.begin(); it != floatingTexts.end();) {
        it->position.x += it->velocity.x * dt;
        it->position.y += it->velocity.y * dt;
        it->velocity.y += 20.0f * dt; // slight downward deceleration
        it->lifetime -= dt;
        it->alpha = std::max(0.0f, it->lifetime / it->maxLifetime);

        if (it->lifetime <= 0.0f) {
            it = floatingTexts.erase(it);
        } else {
            ++it;
        }
    }
}

void ParticleSystem::Draw() {
    // 1. Draw weather particles
    for (const auto& p : weatherParticles) {
        Color c = p.color;
        c.a = (unsigned char)(p.alpha * 255.0f);
        if (p.velocity.y > 200.0f) { // rain streaks
            DrawLineEx(p.position, (Vector2){ p.position.x + p.velocity.x * 0.02f, p.position.y + 10.0f }, p.size, c);
        } else if (p.velocity.x > 300.0f) { // wind streaks
            DrawLineEx(p.position, (Vector2){ p.position.x - 30.0f, p.position.y }, p.size, c);
        } else {
            DrawCircleV(p.position, p.size, c);
        }
    }

    // 2. Draw combat effect particles
    for (const auto& p : particles) {
        Color c = p.color;
        c.a = (unsigned char)(p.alpha * 255.0f);
        if (p.isSpark) {
            DrawCircleV(p.position, p.size, c);
            DrawCircleV(p.position, p.size * 0.5f, WHITE);
        } else {
            DrawRectangleV(p.position, (Vector2){ p.size, p.size }, c);
        }
    }

    // 3. Draw screen flash (for lightning or massive explosion)
    if (lightningFlashTimer > 0.0f) {
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), (Color){ 255, 255, 255, 75 });
    }

    // 4. Draw floating damage / reaction texts
    for (const auto& ft : floatingTexts) {
        Color textColor = ft.color;
        textColor.a = (unsigned char)(ft.alpha * 255.0f);
        Color shadowColor = (Color){ 10, 10, 15, (unsigned char)(ft.alpha * 200.0f) };

        int textWidth = MeasureText(ft.text.c_str(), (int)ft.fontSize);
        Vector2 drawPos = { ft.position.x - textWidth * 0.5f, ft.position.y };

        // Shadow outline
        DrawText(ft.text.c_str(), (int)drawPos.x + 2, (int)drawPos.y + 2, (int)ft.fontSize, shadowColor);
        DrawText(ft.text.c_str(), (int)drawPos.x, (int)drawPos.y, (int)ft.fontSize, textColor);
    }
}
