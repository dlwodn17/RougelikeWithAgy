#include "Systems/CombatSystem.hpp"
#include <random>

static float GetTimeInSeconds() {
    return (float)GetTime();
}

CombatSystem::CombatSystem() 
    : particleSystem(nullptr), currentPhase(CombatPhase::PLAYER_INPUT), phaseTimer(0.0f),
      currentEnemyActionIndex(0), selectedSkillIndex(0), selectedTargetIndex(0),
      selectedStance(StanceType::ATTACK), currentWave(1), maxWaves(3), turnCounter(1) {
    InitializeNewRun();
}

void CombatSystem::InitializeNewRun() {
    player = Player("Arcane Duelist", 100);
    player.SetPosition(370.0f, 470.0f);
    currentWave = 1;
    turnCounter = 1;
    selectedSkillIndex = 0;
    selectedTargetIndex = 0;
    selectedStance = StanceType::ATTACK;
    combatLog.clear();

    if (particleSystem) {
        particleSystem->ClearAll();
    }

    weatherSystem.Initialize(WeatherType::CLEAR);
    player.ResetCooldowns();

    AddCombatLog("=== RUN STARTED: Tower of Elemental Convergence ===", ColorRGBA{ 241, 196, 15, 255 });
    AddCombatLog("Tip: Combine elements (e.g. WET + LIGHTNING = SHOCK) to exploit weaknesses!", ColorRGBA{ 189, 195, 199, 255 });

    StartWave(currentWave);
}

void CombatSystem::StartWave(int waveNumber) {
    currentWave = waveNumber;
    enemies.clear();
    selectedTargetIndex = 0;

    if (waveNumber == 1) {
        AddCombatLog("--- WAVE 1 / 3: Apprentice & Slime ---", ColorRGBA{ 52, 152, 219, 255 });
        Enemy slime("Aquamancer Slime", "Aquamancer Slime", 45, ColorRGBA{ 52, 152, 219, 255 }, 1);
        slime.SetPosition(1260.0f, 470.0f);

        Enemy pyro("Pyromancer", "Pyromancer", 55, ColorRGBA{ 231, 76, 60, 255 }, 1);
        pyro.SetPosition(2040.0f, 470.0f);

        enemies.push_back(slime);
        enemies.push_back(pyro);
    } else if (waveNumber == 2) {
        AddCombatLog("--- WAVE 2 / 3: Storm & Frost Vanguard ---", ColorRGBA{ 155, 89, 182, 255 });
        Enemy slime("Aquamancer Slime", "Aquamancer Slime", 55, ColorRGBA{ 52, 152, 219, 255 }, 2);
        slime.SetPosition(1005.0f, 470.0f);

        Enemy harpy("Storm Harpy", "Storm Harpy", 65, ColorRGBA{ 241, 196, 15, 255 }, 2);
        harpy.SetPosition(1605.0f, 470.0f);

        Enemy golem("Frost Golem", "Frost Golem", 85, ColorRGBA{ 162, 222, 255, 255 }, 2);
        golem.SetPosition(2205.0f, 470.0f);

        enemies.push_back(slime);
        enemies.push_back(harpy);
        enemies.push_back(golem);
    } else {
        AddCombatLog("--- WAVE 3 / 3: BOSS: Elemental Archon ---", ColorRGBA{ 231, 76, 60, 255 });
        Enemy harpy("Storm Minion", "Storm Harpy", 60, ColorRGBA{ 241, 196, 15, 255 }, 3);
        harpy.SetPosition(1005.0f, 470.0f);

        Enemy boss("Elemental Archon", "Elemental Archon", 180, ColorRGBA{ 230, 126, 34, 255 }, 3);
        boss.SetPosition(1605.0f, 470.0f);

        Enemy pyro("Pyro Minion", "Pyromancer", 65, ColorRGBA{ 231, 76, 60, 255 }, 3);
        pyro.SetPosition(2205.0f, 470.0f);

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

void CombatSystem::AddCombatLog(const std::string& text, ColorRGBA color) {
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
    if (index >= 0 && index < static_cast<int>(player.GetSkills().size())) {
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
    if (!player.CanUseSkill(selectedSkillIndex)) {
        AddCombatLog("Cannot use skill: On Cooldown!", ColorRGBA{ 231, 76, 60, 255 });
        return false;
    }

    // Ensure target is valid
    if (selectedTargetIndex < 0 || selectedTargetIndex >= static_cast<int>(enemies.size()) || !enemies[selectedTargetIndex].IsAlive()) {
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
        if (particleSystem) particleSystem->AddFloatingText(player.GetPosition(), "+18 SHIELD", (Color){ 52, 152, 219, 255 });
        AddCombatLog("Player enters Defense Stance (+18 Shield, -30% Dmg).", ColorRGBA{ 52, 152, 219, 255 });
    } else if (selectedStance == StanceType::ATTACK) {
        AddCombatLog("Player enters Attack Stance (+40% Outgoing Dmg).", ColorRGBA{ 231, 76, 60, 255 });
    } else if (selectedStance == StanceType::PARRY) {
        AddCombatLog("Player enters Parry Stance (Status Reflect & Counter).", ColorRGBA{ 241, 196, 15, 255 });
    }

    currentPhase = CombatPhase::RESOLVE_PLAYER_ACTION;
    phaseTimer = 0.5f;
    return true;
}

// Step 1: Resolve Player Action & Elemental Reactions
void CombatSystem::Step1_ResolvePlayerAction() {
    Skill* skill = player.GetSkill(selectedSkillIndex);
    if (!skill) return;

    player.UseSkill(selectedSkillIndex);

    Enemy& target = enemies[selectedTargetIndex];
    int rawDmg = skill->GetEffectiveDamage();

    if (selectedStance == StanceType::ATTACK) {
        rawDmg = static_cast<int>(rawDmg * 1.40f);
    }

    WeatherType weather = weatherSystem.GetCurrentWeather();
    float weatherMult = 1.0f;
    Element effElem = skill->GetEffectiveElement();
    if (effElem == Element::FIRE && weather == WeatherType::HEATWAVE) weatherMult = 1.50f;
    else if (effElem == Element::WET && weather == WeatherType::RAIN) weatherMult = 1.35f;
    else if (effElem == Element::COLD && weather == WeatherType::BLIZZARD) weatherMult = 1.35f;
    else if (effElem == Element::LIGHTNING && weather == WeatherType::THUNDERSTORM) weatherMult = 1.30f;

    DamageReport report = target.ApplyIncomingDamage(rawDmg, effElem, StanceType::ATTACK, weatherMult);

    Element secElem = skill->GetEffectiveSecondaryElement();
    if (secElem != Element::NONE && target.IsAlive()) {
        DamageReport secReport = target.ApplyIncomingDamage(0, secElem, StanceType::ATTACK, weatherMult);
        if (secReport.reaction.triggered) {
            report.reaction = secReport.reaction;
        }
    }

    Color themeCol = ToRaylibColor(skill->GetThemeColor());
    if (particleSystem) {
        particleSystem->SpawnSlashEffect(player.GetPosition(), target.GetPosition(), themeCol);
        particleSystem->SpawnHitSparks(target.GetPosition(), effElem, 15);
        particleSystem->AddFloatingText(target.GetPosition(), "-" + std::to_string(report.mitigatedDamage), themeCol, 26.0f);
    }

    AddCombatLog("Player cast [" + skill->GetName() + "] on " + target.GetName() + " for " + std::to_string(report.mitigatedDamage) + " dmg.", skill->GetThemeColor());

    if (report.reaction.triggered) {
        Color reactionColor = (Color){ 241, 196, 15, 255 };
        if (report.reaction.type == ReactionType::EXPLOSION) reactionColor = (Color){ 231, 76, 60, 255 };
        else if (report.reaction.type == ReactionType::FROZEN) reactionColor = (Color){ 162, 222, 255, 255 };
        else if (report.reaction.type == ReactionType::MELT) reactionColor = (Color){ 243, 156, 18, 255 };
        else if (report.reaction.type == ReactionType::PLASMA) reactionColor = (Color){ 155, 89, 182, 255 };

        if (particleSystem) {
            particleSystem->SpawnReactionBurst(target.GetPosition(), report.reaction.name, reactionColor);
            particleSystem->AddFloatingText(
                (Vector2){ target.GetPosition().x, target.GetPosition().y - 35.0f },
                "★ REACTION: " + report.reaction.name + "!",
                reactionColor,
                24.0f,
                1.6f
            );
        }
        AddCombatLog("[REACTION TRIGGERED: " + report.reaction.name + "] " + report.reaction.description, ColorRGBA{ reactionColor.r, reactionColor.g, reactionColor.b, reactionColor.a });

        if (report.reaction.chainAoE) {
            for (size_t i = 0; i < enemies.size(); ++i) {
                if (static_cast<int>(i) != selectedTargetIndex && enemies[i].IsAlive()) {
                    DamageReport chainReport = enemies[i].ApplyIncomingDamage(report.reaction.aoeDamage, Element::LIGHTNING);
                    if (particleSystem) {
                        particleSystem->SpawnHitSparks(enemies[i].GetPosition(), Element::LIGHTNING, 10);
                        particleSystem->AddFloatingText(enemies[i].GetPosition(), "-" + std::to_string(chainReport.mitigatedDamage) + " ARC", (Color){ 241, 196, 15, 255 });
                    }
                    AddCombatLog("-> Shockwave arcs to " + enemies[i].GetName() + " for " + std::to_string(chainReport.mitigatedDamage) + " dmg!", ColorRGBA{ 241, 196, 15, 255 });
                }
            }
        }
    }

    if (report.causedDeath) {
        AddCombatLog(target.GetName() + " was defeated!", ColorRGBA{ 46, 204, 113, 255 });
        if (particleSystem) particleSystem->AddFloatingText(target.GetPosition(), "DEFEATED", (Color){ 231, 76, 60, 255 }, 28.0f);
    }
}

// Step 2: Resolve Enemy AI Action & Reactions
void CombatSystem::Step2_ResolveEnemyActionStep() {
    if (currentEnemyActionIndex >= static_cast<int>(enemies.size())) {
        currentPhase = CombatPhase::TICK_STATUS_EFFECTS;
        phaseTimer = 0.4f;
        return;
    }

    Enemy& enemy = enemies[currentEnemyActionIndex];
    if (!enemy.IsAlive()) {
        currentEnemyActionIndex++;
        return;
    }

    if (enemy.IsFrozen()) {
        enemy.SetFrozen(false);
        if (particleSystem) particleSystem->AddFloatingText(enemy.GetPosition(), "THAWED (Turn Skipped)", (Color){ 162, 222, 255, 255 });
        AddCombatLog(enemy.GetName() + " is frozen and skips their action!", ColorRGBA{ 162, 222, 255, 255 });
        currentEnemyActionIndex++;
        return;
    }

    const Intent& intent = enemy.GetIntent();

    if (intent.type == IntentType::ATTACK) {
        DamageReport pReport = player.ApplyIncomingDamage(intent.value, intent.element, player.GetStance());

        if (particleSystem) {
            particleSystem->SpawnSlashEffect(enemy.GetPosition(), player.GetPosition(), ToRaylibColor(enemy.GetColor()));
            particleSystem->SpawnHitSparks(player.GetPosition(), intent.element, 10);
        }

        if (pReport.wasParried) {
            int counterDmg = 12;
            DamageReport counterReport = enemy.ApplyIncomingDamage(counterDmg, intent.element);
            if (intent.element != Element::NONE) {
                enemy.ApplyElement(intent.element, 2);
            }
            if (particleSystem) {
                particleSystem->AddFloatingText(player.GetPosition(), "PARRIED! -50%", (Color){ 241, 196, 15, 255 });
                particleSystem->AddFloatingText(enemy.GetPosition(), "-" + std::to_string(counterReport.mitigatedDamage) + " COUNTER", (Color){ 241, 196, 15, 255 });
            }
            AddCombatLog("⚔️ PARRY! Player deflected " + enemy.GetName() + "'s attack and countered for " + std::to_string(counterReport.mitigatedDamage) + " dmg!", ColorRGBA{ 241, 196, 15, 255 });
        } else if (pReport.shieldAbsorbed > 0) {
            if (particleSystem) {
                particleSystem->AddFloatingText(player.GetPosition(), "SHIELDED (" + std::to_string(pReport.shieldAbsorbed) + ")", (Color){ 52, 152, 219, 255 });
                if (pReport.healthDamage > 0) {
                    particleSystem->AddFloatingText(player.GetPosition(), "-" + std::to_string(pReport.healthDamage), (Color){ 231, 76, 60, 255 });
                }
            }
            AddCombatLog(enemy.GetName() + " attacks for " + std::to_string(pReport.mitigatedDamage) + " dmg (Shield absorbed " + std::to_string(pReport.shieldAbsorbed) + ").", ColorRGBA{ 231, 76, 60, 255 });
        } else {
            if (particleSystem) particleSystem->AddFloatingText(player.GetPosition(), "-" + std::to_string(pReport.healthDamage), (Color){ 231, 76, 60, 255 });
            AddCombatLog(enemy.GetName() + " hits Player for " + std::to_string(pReport.healthDamage) + " dmg.", ColorRGBA{ 231, 76, 60, 255 });
        }
    } else if (intent.type == IntentType::DEFEND) {
        enemy.AddShield(intent.value);
        if (particleSystem) particleSystem->AddFloatingText(enemy.GetPosition(), "+" + std::to_string(intent.value) + " SHIELD", (Color){ 52, 152, 219, 255 });
        AddCombatLog(enemy.GetName() + " casts [" + intent.name + "] gaining " + std::to_string(intent.value) + " Shield.", ColorRGBA{ 52, 152, 219, 255 });
    } else if (intent.type == IntentType::DEBUFF) {
        if (player.GetStance() == StanceType::PARRY) {
            enemy.ApplyElement(intent.element, 2);
            if (particleSystem) particleSystem->AddFloatingText(player.GetPosition(), "PARRIED DEBUFF!", (Color){ 241, 196, 15, 255 });
            AddCombatLog("⚔️ PARRY! Player reflected [" + intent.name + "] back to " + enemy.GetName() + "!", ColorRGBA{ 241, 196, 15, 255 });
        } else {
            player.ApplyElement(intent.element, 2);
            if (particleSystem) particleSystem->AddFloatingText(player.GetPosition(), "+" + std::string(ElementalSystem::GetElementName(intent.element)), ToRaylibColor(ElementalSystem::GetElementColor(intent.element)));
            AddCombatLog(enemy.GetName() + " casts [" + intent.name + "] inflicting [" + ElementalSystem::GetElementName(intent.element) + "] on Player.", ElementalSystem::GetElementColor(intent.element));
        }
    } else if (intent.type == IntentType::BUFF) {
        enemy.Heal(intent.value);
        if (particleSystem) particleSystem->AddFloatingText(enemy.GetPosition(), "+" + std::to_string(intent.value) + " HP", (Color){ 46, 204, 113, 255 });
        AddCombatLog(enemy.GetName() + " channels [" + intent.name + "].", ColorRGBA{ 46, 204, 113, 255 });
    }

    enemy.AdvancePattern();
    currentEnemyActionIndex++;
}

// Step 3: Tick Status Effects / Buffs / Debuffs on all Entities
void CombatSystem::Step3_TickStatusEffects() {
    std::vector<std::string> tickLogs;
    player.TickStatusEffects(tickLogs);
    for (auto& enemy : enemies) {
        if (enemy.IsAlive()) {
            enemy.TickStatusEffects(tickLogs);
        }
    }

    for (const auto& log : tickLogs) {
        AddCombatLog(log, ColorRGBA{ 230, 126, 34, 255 });
    }

    player.ResetShield();
    for (auto& enemy : enemies) {
        enemy.ResetShield();
    }
}

// Step 4: Tick Player & Enemy Skill Cooldowns
void CombatSystem::Step4_TickCooldowns() {
    player.TickCooldowns();
    AddCombatLog("Skill cooldowns updated (-1 Turn).", ColorRGBA{ 160, 175, 200, 255 });
}

// Step 5: Advance Weather Forecast Queue & Apply Environmental Effect
void CombatSystem::Step5_AdvanceWeatherAndApply() {
    WeatherType newActiveWeather = weatherSystem.AdvanceTurn();
    WeatherTriggerResult weatherRes = weatherSystem.ProcessTurnStartWeather();

    AddCombatLog("[WEATHER SHIFT: " + weatherRes.title + "] " + weatherRes.description, weatherRes.weatherColor);

    if (weatherRes.globalStatusToApply != Element::NONE) {
        player.ApplyElement(weatherRes.globalStatusToApply, weatherRes.statusDuration);
        if (particleSystem) particleSystem->AddFloatingText(player.GetPosition(), "+" + std::string(ElementalSystem::GetElementName(weatherRes.globalStatusToApply)), ToRaylibColor(weatherRes.weatherColor));

        for (auto& enemy : enemies) {
            if (enemy.IsAlive()) {
                if (newActiveWeather == WeatherType::BLIZZARD && enemy.HasElement(Element::WET)) {
                    enemy.ClearElement(Element::WET);
                    enemy.SetFrozen(true);
                    if (particleSystem) particleSystem->AddFloatingText(enemy.GetPosition(), "FROZEN!", (Color){ 162, 222, 255, 255 }, 24.0f);
                    AddCombatLog(enemy.GetName() + " is drenched and freezes solid in the blizzard!", ColorRGBA{ 162, 222, 255, 255 });
                } else {
                    enemy.ApplyElement(weatherRes.globalStatusToApply, weatherRes.statusDuration);
                    if (particleSystem) particleSystem->AddFloatingText(enemy.GetPosition(), "+" + std::string(ElementalSystem::GetElementName(weatherRes.globalStatusToApply)), ToRaylibColor(weatherRes.weatherColor));
                }
            }
        }
    }

    if (weatherRes.strikeRandomEnemy) {
        std::vector<int> aliveIndices;
        for (size_t i = 0; i < enemies.size(); ++i) {
            if (enemies[i].IsAlive()) aliveIndices.push_back((int)i);
        }
        if (!aliveIndices.empty()) {
            int targetIdx = aliveIndices[rand() % aliveIndices.size()];
            DamageReport strike = enemies[targetIdx].ApplyIncomingDamage(weatherRes.randomStrikeDamage, Element::LIGHTNING);
            if (particleSystem) {
                particleSystem->TriggerScreenFlash(0.15f);
                particleSystem->SpawnHitSparks(enemies[targetIdx].GetPosition(), Element::LIGHTNING, 20);
                particleSystem->AddFloatingText(enemies[targetIdx].GetPosition(), "-" + std::to_string(strike.mitigatedDamage) + " LIGHTNING", (Color){ 241, 196, 15, 255 });
            }
            AddCombatLog("⚡ Lightning bolt strikes " + enemies[targetIdx].GetName() + " for " + std::to_string(strike.mitigatedDamage) + " dmg!", ColorRGBA{ 241, 196, 15, 255 });
        }
    }

    if (weatherRes.spreadAllDebuffs) {
        std::vector<Element> collectedElements;
        for (const auto& enemy : enemies) {
            if (enemy.IsAlive()) {
                auto spread = ElementalSystem::GetActiveElementsFromMask(enemy.GetActiveStatusMask());
                collectedElements.insert(collectedElements.end(), spread.begin(), spread.end());
            }
        }
        for (Element elem : collectedElements) {
            for (auto& enemy : enemies) {
                if (enemy.IsAlive()) enemy.ApplyElement(elem, 2);
            }
        }
        if (!collectedElements.empty()) {
            AddCombatLog("🌪️ Gale winds swirl statuses across all combatants!", ColorRGBA{ 46, 204, 113, 255 });
        }
    }
}

// Step 6: Reset Stances / Action Points and start Next Turn
void CombatSystem::Step6_ResetTurnAndStartNext() {
    turnCounter++;

    for (auto& enemy : enemies) {
        if (enemy.IsAlive()) {
            enemy.DecideIntent(turnCounter, player, weatherSystem.GetCurrentWeather());
        }
    }

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
        AddCombatLog("=== DEFEAT: You have fallen in combat ===", ColorRGBA{ 231, 76, 60, 255 });
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
        AddCombatLog("=== VICTORY: Wave Cleared! ===", ColorRGBA{ 46, 204, 113, 255 });
    } else {
        currentPhase = CombatPhase::PLAYER_INPUT;
    }
}

void CombatSystem::NextWave() {
    if (currentWave < maxWaves) {
        currentWave++;
        player.Heal(25);
        player.ResetCooldowns();
        StartWave(currentWave);
    } else {
        RestartGame();
    }
}

void CombatSystem::RestartGame() {
    InitializeNewRun();
}

void CombatSystem::Update(float dt) {
    player.UpdateVisuals(dt);
    for (auto& enemy : enemies) {
        enemy.UpdateVisuals(dt);
    }

    if (phaseTimer > 0.0f) {
        phaseTimer -= dt;
        return;
    }

    switch (currentPhase) {
        case CombatPhase::RESOLVE_PLAYER_ACTION:
            Step1_ResolvePlayerAction();
            CheckBattleEndConditions();
            if (currentPhase != CombatPhase::VICTORY_SCREEN && currentPhase != CombatPhase::DEFEAT_SCREEN) {
                currentPhase = CombatPhase::RESOLVE_ENEMY_ACTIONS;
                currentEnemyActionIndex = 0;
                phaseTimer = 0.4f;
            }
            break;

        case CombatPhase::RESOLVE_ENEMY_ACTIONS:
            Step2_ResolveEnemyActionStep();
            CheckBattleEndConditions();
            if (currentPhase == CombatPhase::RESOLVE_ENEMY_ACTIONS) {
                phaseTimer = 0.45f;
            }
            break;

        case CombatPhase::TICK_STATUS_EFFECTS:
            Step3_TickStatusEffects();
            CheckBattleEndConditions();
            if (currentPhase != CombatPhase::VICTORY_SCREEN && currentPhase != CombatPhase::DEFEAT_SCREEN) {
                currentPhase = CombatPhase::TICK_COOLDOWNS;
                phaseTimer = 0.3f;
            }
            break;

        case CombatPhase::TICK_COOLDOWNS:
            Step4_TickCooldowns();
            currentPhase = CombatPhase::ADVANCE_WEATHER_AND_APPLY;
            phaseTimer = 0.4f;
            break;

        case CombatPhase::ADVANCE_WEATHER_AND_APPLY:
            Step5_AdvanceWeatherAndApply();
            CheckBattleEndConditions();
            if (currentPhase != CombatPhase::VICTORY_SCREEN && currentPhase != CombatPhase::DEFEAT_SCREEN) {
                currentPhase = CombatPhase::RESET_TURN_AND_START_NEXT;
                phaseTimer = 0.3f;
            }
            break;

        case CombatPhase::RESET_TURN_AND_START_NEXT:
            Step6_ResetTurnAndStartNext();
            break;

        case CombatPhase::PLAYER_INPUT:
        case CombatPhase::VICTORY_SCREEN:
        case CombatPhase::DEFEAT_SCREEN:
        default:
            break;
    }
}
