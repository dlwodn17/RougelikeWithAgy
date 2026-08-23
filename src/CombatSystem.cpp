#include "CombatSystem.hpp"
#include <random>

static float GetTimeInSeconds() {
    return (float)GetTime();
}

CombatSystem::CombatSystem() 
    : currentPhase(CombatPhase::PLAYER_INPUT), phaseTimer(0.0f), currentEnemyActionIndex(0),
      selectedSkillIndex(0), selectedTargetIndex(0), selectedStance(StanceType::ATTACK),
      currentWave(1), maxWaves(3), turnCounter(1) {
    InitializeNewRun();
}

void CombatSystem::InitializeNewRun() {
    player = Player("Arcane Duelist", 100);
    player.SetPosition((Vector2){ 260.0f, 320.0f });
    currentWave = 1;
    turnCounter = 1;
    selectedSkillIndex = 0;
    selectedTargetIndex = 0;
    selectedStance = StanceType::ATTACK;
    combatLog.clear();
    particleSystem.ClearAll();

    weatherSystem.Initialize(WeatherType::CLEAR);
    skillSystem.InitializeDefaultSkills();

    AddCombatLog("=== RUN STARTED: Tower of Elemental Convergence ===", (Color){ 241, 196, 15, 255 });
    AddCombatLog("Tip: Combine elements (e.g. WET + LIGHTNING = SHOCK) to exploit weaknesses!", (Color){ 189, 195, 199, 255 });

    StartWave(currentWave);
}

void CombatSystem::StartWave(int waveNumber) {
    currentWave = waveNumber;
    enemies.clear();
    selectedTargetIndex = 0;

    int screenWidth = 1280;
    int screenHeight = 720;
    (void)screenHeight;

    if (waveNumber == 1) {
        AddCombatLog("--- WAVE 1 / 3: Apprentice & Slime ---", (Color){ 52, 152, 219, 255 });
        Enemy slime("Aquamancer Slime", "Aquamancer Slime", 45, (Color){ 52, 152, 219, 255 }, 1);
        slime.SetPosition((Vector2){ (float)screenWidth * 0.58f, 300.0f });

        Enemy pyro("Pyromancer", "Pyromancer", 55, (Color){ 231, 76, 60, 255 }, 1);
        pyro.SetPosition((Vector2){ (float)screenWidth * 0.80f, 300.0f });

        enemies.push_back(slime);
        enemies.push_back(pyro);
    } else if (waveNumber == 2) {
        AddCombatLog("--- WAVE 2 / 3: Storm & Frost Vanguard ---", (Color){ 155, 89, 182, 255 });
        Enemy slime("Aquamancer Slime", "Aquamancer Slime", 55, (Color){ 52, 152, 219, 255 }, 2);
        slime.SetPosition((Vector2){ (float)screenWidth * 0.52f, 300.0f });

        Enemy harpy("Storm Harpy", "Storm Harpy", 65, (Color){ 241, 196, 15, 255 }, 2);
        harpy.SetPosition((Vector2){ (float)screenWidth * 0.70f, 250.0f });

        Enemy golem("Frost Golem", "Frost Golem", 85, (Color){ 162, 222, 255, 255 }, 2);
        golem.SetPosition((Vector2){ (float)screenWidth * 0.88f, 320.0f });

        enemies.push_back(slime);
        enemies.push_back(harpy);
        enemies.push_back(golem);
    } else {
        AddCombatLog("--- WAVE 3 / 3: BOSS: Elemental Archon ---", (Color){ 231, 76, 60, 255 });
        Enemy harpy("Storm Minion", "Storm Harpy", 60, (Color){ 241, 196, 15, 255 }, 3);
        harpy.SetPosition((Vector2){ (float)screenWidth * 0.52f, 340.0f });

        Enemy boss("Elemental Archon", "Elemental Archon", 180, (Color){ 230, 126, 34, 255 }, 3);
        boss.SetPosition((Vector2){ (float)screenWidth * 0.72f, 280.0f });

        Enemy pyro("Pyro Minion", "Pyromancer", 65, (Color){ 231, 76, 60, 255 }, 3);
        pyro.SetPosition((Vector2){ (float)screenWidth * 0.90f, 340.0f });

        enemies.push_back(harpy);
        enemies.push_back(boss);
        enemies.push_back(pyro);
    }

    // Decide initial enemy intents
    for (auto& enemy : enemies) {
        enemy.DecideIntent(turnCounter, player, weatherSystem.GetCurrentWeather());
    }

    currentPhase = CombatPhase::PLAYER_INPUT;
}

void CombatSystem::AddCombatLog(const std::string& text, Color color) {
    CombatLogEntry entry;
    entry.text = text;
    entry.color = color;
    entry.timestamp = GetTimeInSeconds();
    combatLog.push_back(entry);

    if (combatLog.size() > 50) {
        combatLog.erase(combatLog.begin());
    }
}

void CombatSystem::SelectSkill(int index) {
    if (currentPhase != CombatPhase::PLAYER_INPUT) return;
    if (index >= 0 && index < static_cast<int>(skillSystem.GetSkills().size())) {
        selectedSkillIndex = index;
    }
}

void CombatSystem::SelectTarget(int index) {
    if (currentPhase != CombatPhase::PLAYER_INPUT) return;
    if (index >= 0 && index < static_cast<int>(enemies.size())) {
        if (enemies[index].IsAlive()) {
            selectedTargetIndex = index;
        }
    }
}

void CombatSystem::SelectStance(StanceType stance) {
    if (currentPhase != CombatPhase::PLAYER_INPUT) return;
    selectedStance = stance;
    player.SetStance(stance);
}

bool CombatSystem::ExecutePlayerTurn() {
    if (currentPhase != CombatPhase::PLAYER_INPUT) return false;
    if (!skillSystem.CanUseSkill(selectedSkillIndex)) {
        AddCombatLog("Cannot use skill: On Cooldown!", (Color){ 231, 76, 60, 255 });
        return false;
    }

    // Ensure target is valid
    if (selectedTargetIndex < 0 || selectedTargetIndex >= static_cast<int>(enemies.size()) || !enemies[selectedTargetIndex].IsAlive()) {
        // Auto pick first alive target
        bool found = false;
        for (size_t i = 0; i < enemies.size(); ++i) {
            if (enemies[i].IsAlive()) {
                selectedTargetIndex = (int)i;
                found = true;
                break;
            }
        }
        if (!found) return false;
    }

    // Apply Stance buffs to Player before action
    player.SetStance(selectedStance);
    if (selectedStance == StanceType::DEFENSE) {
        player.AddShield(18);
        particleSystem.AddFloatingText(player.GetPosition(), "+18 SHIELD", (Color){ 52, 152, 219, 255 });
        AddCombatLog("Player enters Defense Stance (+18 Shield, -30% Dmg).", (Color){ 52, 152, 219, 255 });
    } else if (selectedStance == StanceType::ATTACK) {
        AddCombatLog("Player enters Attack Stance (+40% Outgoing Dmg).", (Color){ 231, 76, 60, 255 });
    } else if (selectedStance == StanceType::PARRY) {
        AddCombatLog("Player enters Parry Stance (Status Reflect & Counter).", (Color){ 241, 196, 15, 255 });
    }

    currentPhase = CombatPhase::ACTION_EXECUTION;
    phaseTimer = 0.6f;
    return true;
}

void CombatSystem::ProcessPlayerAction() {
    Skill* skill = skillSystem.GetSkill(selectedSkillIndex);
    if (!skill) return;

    skillSystem.UseSkill(selectedSkillIndex);

    Enemy& target = enemies[selectedTargetIndex];
    int rawDmg = skill->baseDamage;

    // Stance damage multiplier
    if (selectedStance == StanceType::ATTACK) {
        rawDmg = static_cast<int>(rawDmg * 1.40f);
    }

    // Weather damage modifier
    WeatherType weather = weatherSystem.GetCurrentWeather();
    float weatherMult = 1.0f;
    if (skill->primaryElement == Element::FIRE && weather == WeatherType::HEATWAVE) weatherMult = 1.50f;
    else if (skill->primaryElement == Element::WET && weather == WeatherType::RAIN) weatherMult = 1.35f;
    else if (skill->primaryElement == Element::COLD && weather == WeatherType::BLIZZARD) weatherMult = 1.35f;
    else if (skill->primaryElement == Element::LIGHTNING && weather == WeatherType::THUNDERSTORM) weatherMult = 1.30f;

    // Execute attack & reaction
    DamageReport report = target.ApplyIncomingDamage(rawDmg, skill->primaryElement, StanceType::ATTACK, weatherMult);

    // Apply secondary element if any (e.g. Ignition Flask applying Oil then Fire)
    if (skill->secondaryElement != Element::NONE && target.IsAlive()) {
        DamageReport secReport = target.ApplyIncomingDamage(0, skill->secondaryElement, StanceType::ATTACK, weatherMult);
        if (secReport.reaction.triggered) {
            report.reaction = secReport.reaction;
        }
    }

    // Visuals & Floating Text
    particleSystem.SpawnSlashEffect(player.GetPosition(), target.GetPosition(), skill->themeColor);
    particleSystem.SpawnHitSparks(target.GetPosition(), skill->primaryElement, 15);
    particleSystem.AddFloatingText(target.GetPosition(), "-" + std::to_string(report.mitigatedDamage), skill->themeColor, 26.0f);

    AddCombatLog("Player cast [" + skill->name + "] on " + target.GetName() + " for " + std::to_string(report.mitigatedDamage) + " dmg.", skill->themeColor);

    // Handle Reaction Triggered!
    if (report.reaction.triggered) {
        particleSystem.SpawnReactionBurst(target.GetPosition(), report.reaction.reactionName, report.reaction.reactionColor);
        particleSystem.AddFloatingText(
            (Vector2){ target.GetPosition().x, target.GetPosition().y - 35.0f },
            "★ REACTION: " + report.reaction.reactionName + "!",
            report.reaction.reactionColor,
            24.0f,
            1.6f
        );
        AddCombatLog("[REACTION TRIGGERED: " + report.reaction.reactionName + "] " + report.reaction.description, report.reaction.reactionColor);

        // Chain AoE to other enemies for SHOCK reaction
        if (report.reaction.chainAoE) {
            for (size_t i = 0; i < enemies.size(); ++i) {
                if (static_cast<int>(i) != selectedTargetIndex && enemies[i].IsAlive()) {
                    DamageReport chainReport = enemies[i].ApplyIncomingDamage(report.reaction.aoeDamage, Element::LIGHTNING);
                    particleSystem.SpawnHitSparks(enemies[i].GetPosition(), Element::LIGHTNING, 10);
                    particleSystem.AddFloatingText(enemies[i].GetPosition(), "-" + std::to_string(chainReport.mitigatedDamage) + " ARC", (Color){ 241, 196, 15, 255 });
                    AddCombatLog("-> Shockwave arcs to " + enemies[i].GetName() + " for " + std::to_string(chainReport.mitigatedDamage) + " dmg!", (Color){ 241, 196, 15, 255 });
                }
            }
        }
    }

    if (report.causedDeath) {
        AddCombatLog(target.GetName() + " was defeated!", (Color){ 46, 204, 113, 255 });
        particleSystem.AddFloatingText(target.GetPosition(), "DEFEATED", (Color){ 231, 76, 60, 255 }, 28.0f);
    }
}

void CombatSystem::ProcessWeatherPhase() {
    WeatherTriggerResult weatherRes = weatherSystem.ProcessTurnStartWeather();
    AddCombatLog("[WEATHER: " + weatherRes.title + "] " + weatherRes.description, weatherRes.weatherColor);

    // Apply global status to combatants if active
    if (weatherRes.globalStatusToApply != Element::NONE) {
        player.InflictElement(weatherRes.globalStatusToApply, weatherRes.statusDuration);
        particleSystem.AddFloatingText(player.GetPosition(), "+" + std::string(ElementalSystem::GetElementName(weatherRes.globalStatusToApply)), weatherRes.weatherColor);

        for (auto& enemy : enemies) {
            if (enemy.IsAlive()) {
                // If Blizzard and target is wet -> instant freeze!
                if (weatherSystem.GetCurrentWeather() == WeatherType::BLIZZARD && enemy.HasElement(Element::WET)) {
                    enemy.ClearElement(Element::WET);
                    enemy.SetFrozen(true);
                    particleSystem.AddFloatingText(enemy.GetPosition(), "FROZEN!", (Color){ 162, 222, 255, 255 }, 24.0f);
                    AddCombatLog(enemy.GetName() + " is drenched and freezes solid in the blizzard!", (Color){ 162, 222, 255, 255 });
                } else {
                    enemy.InflictElement(weatherRes.globalStatusToApply, weatherRes.statusDuration);
                    particleSystem.AddFloatingText(enemy.GetPosition(), "+" + std::string(ElementalSystem::GetElementName(weatherRes.globalStatusToApply)), weatherRes.weatherColor);
                }
            }
        }
    }

    // Strike random target for Thunderstorm
    if (weatherRes.strikeRandomEnemy) {
        std::vector<int> aliveIndices;
        for (size_t i = 0; i < enemies.size(); ++i) {
            if (enemies[i].IsAlive()) aliveIndices.push_back((int)i);
        }
        if (!aliveIndices.empty()) {
            int targetIdx = aliveIndices[rand() % aliveIndices.size()];
            DamageReport strike = enemies[targetIdx].ApplyIncomingDamage(weatherRes.randomStrikeDamage, Element::LIGHTNING);
            particleSystem.TriggerScreenFlash(0.15f);
            particleSystem.SpawnHitSparks(enemies[targetIdx].GetPosition(), Element::LIGHTNING, 20);
            particleSystem.AddFloatingText(enemies[targetIdx].GetPosition(), "-" + std::to_string(strike.mitigatedDamage) + " LIGHTNING", (Color){ 241, 196, 15, 255 });
            AddCombatLog("⚡ Lightning bolt strikes " + enemies[targetIdx].GetName() + " for " + std::to_string(strike.mitigatedDamage) + " dmg!", (Color){ 241, 196, 15, 255 });
        }
    }

    // Spread debuffs for Gale Winds
    if (weatherRes.spreadAllDebuffs) {
        std::vector<Element> collectedElements;
        for (const auto& enemy : enemies) {
            if (enemy.IsAlive()) {
                auto spread = ElementalSystem::GetSpreadElements(enemy.GetStatusBuffer());
                collectedElements.insert(collectedElements.end(), spread.begin(), spread.end());
            }
        }
        for (Element elem : collectedElements) {
            for (auto& enemy : enemies) {
                if (enemy.IsAlive()) enemy.InflictElement(elem, 2);
            }
        }
        if (!collectedElements.empty()) {
            AddCombatLog("🌪️ Gale winds swirl statuses across all combatants!", (Color){ 46, 204, 113, 255 });
        }
    }
}

void CombatSystem::ProcessEnemyActionStep() {
    if (currentEnemyActionIndex >= static_cast<int>(enemies.size())) {
        currentPhase = CombatPhase::STATUS_TICK;
        phaseTimer = 0.5f;
        return;
    }

    Enemy& enemy = enemies[currentEnemyActionIndex];
    if (!enemy.IsAlive()) {
        currentEnemyActionIndex++;
        return;
    }

    // Check Frozen / Stunned
    if (enemy.IsFrozen()) {
        enemy.SetFrozen(false);
        particleSystem.AddFloatingText(enemy.GetPosition(), "THAWED (Turn Skipped)", (Color){ 162, 222, 255, 255 });
        AddCombatLog(enemy.GetName() + " is frozen and skips their action!", (Color){ 162, 222, 255, 255 });
        currentEnemyActionIndex++;
        return;
    }

    const Intent& intent = enemy.GetIntent();

    if (intent.type == IntentType::ATTACK) {
        // Player mitigation & Parry Check
        DamageReport pReport = player.ApplyIncomingDamage(intent.value, intent.element, player.GetStance());

        particleSystem.SpawnSlashEffect(enemy.GetPosition(), player.GetPosition(), enemy.GetColor());
        particleSystem.SpawnHitSparks(player.GetPosition(), intent.element, 10);

        if (pReport.wasParried) {
            // Parry reflect & counter!
            int counterDmg = 12;
            DamageReport counterReport = enemy.ApplyIncomingDamage(counterDmg, intent.element);
            if (intent.element != Element::NONE) {
                enemy.InflictElement(intent.element, 2);
            }
            particleSystem.AddFloatingText(player.GetPosition(), "PARRIED! -50%", (Color){ 241, 196, 15, 255 });
            particleSystem.AddFloatingText(enemy.GetPosition(), "-" + std::to_string(counterReport.mitigatedDamage) + " COUNTER", (Color){ 241, 196, 15, 255 });
            AddCombatLog("⚔️ PARRY! Player deflected " + enemy.GetName() + "'s attack and countered for " + std::to_string(counterReport.mitigatedDamage) + " dmg!", (Color){ 241, 196, 15, 255 });
        } else if (pReport.shieldAbsorbed > 0) {
            particleSystem.AddFloatingText(player.GetPosition(), "SHIELDED (" + std::to_string(pReport.shieldAbsorbed) + ")", (Color){ 52, 152, 219, 255 });
            if (pReport.healthDamage > 0) {
                particleSystem.AddFloatingText(player.GetPosition(), "-" + std::to_string(pReport.healthDamage), (Color){ 231, 76, 60, 255 });
            }
            AddCombatLog(enemy.GetName() + " attacks for " + std::to_string(pReport.mitigatedDamage) + " dmg (Shield absorbed " + std::to_string(pReport.shieldAbsorbed) + ").", (Color){ 231, 76, 60, 255 });
        } else {
            particleSystem.AddFloatingText(player.GetPosition(), "-" + std::to_string(pReport.healthDamage), (Color){ 231, 76, 60, 255 });
            AddCombatLog(enemy.GetName() + " hits Player for " + std::to_string(pReport.healthDamage) + " dmg.", (Color){ 231, 76, 60, 255 });
        }
    } else if (intent.type == IntentType::DEFEND) {
        enemy.AddShield(intent.value);
        particleSystem.AddFloatingText(enemy.GetPosition(), "+" + std::to_string(intent.value) + " SHIELD", (Color){ 52, 152, 219, 255 });
        AddCombatLog(enemy.GetName() + " casts [" + intent.name + "] gaining " + std::to_string(intent.value) + " Shield.", (Color){ 52, 152, 219, 255 });
    } else if (intent.type == IntentType::DEBUFF) {
        if (player.GetStance() == StanceType::PARRY) {
            enemy.InflictElement(intent.element, 2);
            particleSystem.AddFloatingText(player.GetPosition(), "PARRIED DEBUFF!", (Color){ 241, 196, 15, 255 });
            AddCombatLog("⚔️ PARRY! Player reflected [" + intent.name + "] back to " + enemy.GetName() + "!", (Color){ 241, 196, 15, 255 });
        } else {
            player.InflictElement(intent.element, 2);
            particleSystem.AddFloatingText(player.GetPosition(), "+" + std::string(ElementalSystem::GetElementName(intent.element)), GetElementBaseColor(intent.element));
            AddCombatLog(enemy.GetName() + " casts [" + intent.name + "] inflicting [" + ElementalSystem::GetElementName(intent.element) + "] on Player.", GetElementBaseColor(intent.element));
        }
    } else if (intent.type == IntentType::BUFF) {
        enemy.Heal(intent.value);
        particleSystem.AddFloatingText(enemy.GetPosition(), "+" + std::to_string(intent.value) + " HP", (Color){ 46, 204, 113, 255 });
        AddCombatLog(enemy.GetName() + " channels [" + intent.name + "].", (Color){ 46, 204, 113, 255 });
    }

    enemy.AdvancePattern();
    currentEnemyActionIndex++;
}

void CombatSystem::ProcessStatusTickPhase() {
    std::vector<std::string> tickLogs;
    player.TickStatusEffects(tickLogs);
    for (auto& enemy : enemies) {
        if (enemy.IsAlive()) {
            enemy.TickStatusEffects(tickLogs);
        }
    }

    for (const auto& log : tickLogs) {
        AddCombatLog(log, (Color){ 230, 126, 34, 255 });
    }

    // Reset 1-turn temp shields
    player.ResetShield();
    for (auto& enemy : enemies) {
        enemy.ResetShield();
    }

    currentPhase = CombatPhase::TURN_END_CLEANUP;
    phaseTimer = 0.4f;
}

void CombatSystem::ProcessTurnEndCleanup() {
    // 1. Skill cooldowns tick down
    skillSystem.TickAllCooldowns();

    // 2. Weather forecast shifts
    weatherSystem.AdvanceForecast();

    // 3. Turn counter increments
    turnCounter++;

    // 4. Check alive enemies and update their next intents
    for (auto& enemy : enemies) {
        if (enemy.IsAlive()) {
            enemy.DecideIntent(turnCounter, player, weatherSystem.GetCurrentWeather());
        }
    }

    // Check if target is still alive, otherwise select first alive
    if (selectedTargetIndex >= 0 && selectedTargetIndex < static_cast<int>(enemies.size()) && !enemies[selectedTargetIndex].IsAlive()) {
        for (size_t i = 0; i < enemies.size(); ++i) {
            if (enemies[i].IsAlive()) {
                selectedTargetIndex = (int)i;
                break;
            }
        }
    }

    CheckBattleEndConditions();
}

void CombatSystem::CheckBattleEndConditions() {
    if (!player.IsAlive()) {
        currentPhase = CombatPhase::DEFEAT_SCREEN;
        AddCombatLog("=== DEFEAT: You have fallen in combat ===", (Color){ 231, 76, 60, 255 });
        return;
    }

    bool allEnemiesDead = true;
    for (const auto& enemy : enemies) {
        if (enemy.IsAlive()) {
            allEnemiesDead = false;
            break;
        }
    }

    if (allEnemiesDead) {
        currentPhase = CombatPhase::VICTORY_SCREEN;
        AddCombatLog("=== VICTORY: Wave Cleared! ===", (Color){ 46, 204, 113, 255 });
    } else {
        currentPhase = CombatPhase::PLAYER_INPUT;
    }
}

void CombatSystem::NextWave() {
    if (currentWave < maxWaves) {
        currentWave++;
        player.Heal(25); // Roguelike wave heal
        StartWave(currentWave);
    } else {
        // Loop or finish
        RestartGame();
    }
}

void CombatSystem::RestartGame() {
    InitializeNewRun();
}

void CombatSystem::Update(float dt) {
    // Update visual offsets and shake timers
    player.UpdateVisuals(dt);
    for (auto& enemy : enemies) {
        enemy.UpdateVisuals(dt);
    }

    particleSystem.Update(dt, weatherSystem.GetCurrentWeather());

    if (phaseTimer > 0.0f) {
        phaseTimer -= dt;
        return;
    }

    // Step through the combat state machine
    switch (currentPhase) {
        case CombatPhase::ACTION_EXECUTION:
            ProcessPlayerAction();
            CheckBattleEndConditions();
            if (currentPhase != CombatPhase::VICTORY_SCREEN && currentPhase != CombatPhase::DEFEAT_SCREEN) {
                currentPhase = CombatPhase::WEATHER_TRIGGER;
                phaseTimer = 0.5f;
            }
            break;

        case CombatPhase::WEATHER_TRIGGER:
            ProcessWeatherPhase();
            CheckBattleEndConditions();
            if (currentPhase != CombatPhase::VICTORY_SCREEN && currentPhase != CombatPhase::DEFEAT_SCREEN) {
                currentPhase = CombatPhase::ENEMY_ACTIONS;
                currentEnemyActionIndex = 0;
                phaseTimer = 0.4f;
            }
            break;

        case CombatPhase::ENEMY_ACTIONS:
            ProcessEnemyActionStep();
            CheckBattleEndConditions();
            if (currentPhase == CombatPhase::ENEMY_ACTIONS) {
                phaseTimer = 0.5f;
            }
            break;

        case CombatPhase::STATUS_TICK:
            ProcessStatusTickPhase();
            break;

        case CombatPhase::TURN_END_CLEANUP:
            ProcessTurnEndCleanup();
            break;

        case CombatPhase::PLAYER_INPUT:
        case CombatPhase::VICTORY_SCREEN:
        case CombatPhase::DEFEAT_SCREEN:
        default:
            break;
    }
}
