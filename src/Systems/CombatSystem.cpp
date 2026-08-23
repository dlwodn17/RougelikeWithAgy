#include "Systems/CombatSystem.hpp"
#include "Core/Localization.hpp"
#include <random>

static float GetTimeInSeconds() {
    return (float)GetTime();
}

CombatSystem::CombatSystem() 
    : particleSystem(nullptr), currentPhase(CombatPhase::PLAYER_INPUT), phaseTimer(0.0f),
      selectedSkillIndex(0), selectedTargetIndex(0), selectedStance(StanceType::ATTACK),
      currentWave(1), maxWaves(3), turnCounter(1) {
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

    if (Localization::IsKorean()) {
        AddCombatLog("=== 원정 시작: 원소 수렴의 탑 (Tower of Convergence) ===", ColorRGBA{ 241, 196, 15, 255 });
        AddCombatLog("전략 팁: 원소 조합(수분+전기=감전, 기름+화염=폭발 등)을 활용해 약점을 공략하세요!", ColorRGBA{ 189, 195, 199, 255 });
    } else {
        AddCombatLog("=== RUN STARTED: Tower of Elemental Convergence ===", ColorRGBA{ 241, 196, 15, 255 });
        AddCombatLog("Tip: Combine elements (e.g. WET + ELEC = SHOCK) to trigger devastating combos!", ColorRGBA{ 189, 195, 199, 255 });
    }

    StartWave(currentWave);
}

void CombatSystem::StartWave(int waveNumber) {
    currentWave = waveNumber;
    enemies.clear();
    selectedTargetIndex = 0;

    if (waveNumber == 1) {
        if (Localization::IsKorean()) AddCombatLog("--- 제 1 / 3 웨이브: 견습 마법사 & 슬라임 ---", ColorRGBA{ 52, 152, 219, 255 });
        else AddCombatLog("--- WAVE 1 / 3: Apprentice & Slime ---", ColorRGBA{ 52, 152, 219, 255 });
        
        Enemy slime("Aquamancer Slime", "Aquamancer Slime", 45, ColorRGBA{ 52, 152, 219, 255 }, 1);
        slime.SetPosition(1260.0f, 470.0f);

        Enemy pyro("Pyromancer", "Pyromancer", 55, ColorRGBA{ 231, 76, 60, 255 }, 1);
        pyro.SetPosition(2040.0f, 470.0f);

        enemies.push_back(slime);
        enemies.push_back(pyro);
    } else if (waveNumber == 2) {
        if (Localization::IsKorean()) AddCombatLog("--- 제 2 / 3 웨이브: 폭풍 & 서리 선봉대 ---", ColorRGBA{ 155, 89, 182, 255 });
        else AddCombatLog("--- WAVE 2 / 3: Storm & Frost Vanguard ---", ColorRGBA{ 155, 89, 182, 255 });

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
        if (Localization::IsKorean()) AddCombatLog("--- 제 3 / 3 웨이브: [보스] 원소의 아콘 ---", ColorRGBA{ 231, 76, 60, 255 });
        else AddCombatLog("--- WAVE 3 / 3: BOSS: Elemental Archon ---", ColorRGBA{ 231, 76, 60, 255 });

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

    if (combatLog.size() > 60) {
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

bool CombatSystem::CheckWaveCleared() {
    for (const auto& enemy : enemies) {
        if (enemy.IsAlive()) return false;
    }
    return true;
}

bool CombatSystem::ExecutePlayerTurn() {
    if (currentPhase != CombatPhase::PLAYER_INPUT) return false;
    
    Skill* skill = player.GetSkill(selectedSkillIndex);
    if (!skill || !skill->IsReady()) {
        int cd = skill ? skill->GetCurrentCooldown() : 0;
        if (Localization::IsKorean()) {
            AddCombatLog("스킬을 사용할 수 없습니다: 쿨다운 중 (" + std::to_string(cd) + "턴 남음)!", ColorRGBA{ 231, 76, 60, 255 });
        } else {
            AddCombatLog("Cannot use skill: On Cooldown (" + std::to_string(cd) + "T remaining)!", ColorRGBA{ 231, 76, 60, 255 });
        }
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

    ExecuteTurn();
    return true;
}

// 1. Resolve Player Action Subroutine
void CombatSystem::ResolvePlayerAction(int skillIdx, int targetIdx, StanceType stance) {
    Skill* skill = player.GetSkill(skillIdx);
    if (!skill) return;

    player.SetStance(stance);
    if (stance == StanceType::DEFENSE) {
        player.AddShield(18);
        if (particleSystem) particleSystem->AddFloatingText(player.GetPosition(), Localization::IsKorean() ? "+18 방어막" : "+18 SHIELD", (Color){ 52, 152, 219, 255 });
        if (Localization::IsKorean()) AddCombatLog("영웅이 방어 태세를 취했습니다 (+18 방어막, 받는 피해 -30%).", ColorRGBA{ 52, 152, 219, 255 });
        else AddCombatLog("Player assumes Defense Stance (+18 Shield, -30% Dmg taken).", ColorRGBA{ 52, 152, 219, 255 });
    } else if (stance == StanceType::ATTACK) {
        if (Localization::IsKorean()) AddCombatLog("영웅이 공격 태세를 취했습니다 (주는 피해 +40%).", ColorRGBA{ 231, 76, 60, 255 });
        else AddCombatLog("Player assumes Attack Stance (+40% Outgoing Dmg).", ColorRGBA{ 231, 76, 60, 255 });
    } else if (stance == StanceType::PARRY) {
        if (Localization::IsKorean()) AddCombatLog("영웅이 패링 태세를 취했습니다 (12 반격 및 상태이상 반사 준비).", ColorRGBA{ 241, 196, 15, 255 });
        else AddCombatLog("Player assumes Parry Stance (Counter-attack & Status Reflect).", ColorRGBA{ 241, 196, 15, 255 });
    }

    player.UseSkill(skillIdx);

    Enemy& target = enemies[targetIdx];
    int rawDmg = skill->GetFinalDamage(player.GetShield());
    if (skill->GetFinalShield() > 0) {
        player.AddShield(skill->GetFinalShield());
    }

    if (stance == StanceType::ATTACK) {
        rawDmg = static_cast<int>(rawDmg * 1.40f);
    }

    WeatherType weather = weatherSystem.GetCurrentWeather();
    float weatherMult = 1.0f;
    Element effElem = skill->GetFinalElement();
    if (effElem == Element::FIRE && weather == WeatherType::HEATWAVE) weatherMult = 1.50f;
    else if (effElem == Element::WET && weather == WeatherType::RAIN) weatherMult = 1.35f;
    else if (effElem == Element::COLD && weather == WeatherType::BLIZZARD) weatherMult = 1.35f;
    else if (effElem == Element::LIGHTNING && weather == WeatherType::STORM) weatherMult = 1.30f;

    DamageReport report = target.ApplyIncomingDamage(rawDmg, effElem, StanceType::ATTACK, weatherMult);

    // Rune Special: Freeze WET target
    if (skill->HasFreezeWet() && target.IsAlive() && target.HasElement(Element::WET)) {
        target.SetFrozen(true);
        target.ClearElement(Element::WET);
        report.reaction.triggered = true;
        report.reaction.type = ReactionType::FROZEN;
        report.reaction.name = "FROZEN";
        report.reaction.description = "Frostfire rune flash-freezes drenched enemy!";
    }

    Element secElem = skill->GetFinalSecondaryElement();
    if (secElem != Element::NONE && target.IsAlive()) {
        DamageReport secReport = target.ApplyIncomingDamage(0, secElem, StanceType::ATTACK, weatherMult);
        if (secReport.reaction.triggered) {
            report.reaction = secReport.reaction;
        }
    }

    Color themeCol = ToRaylibColor(skill->GetEffectiveThemeColor());
    if (particleSystem) {
        particleSystem->SpawnSlashEffect(player.GetPosition(), target.GetPosition(), themeCol);
        particleSystem->SpawnHitSparks(target.GetPosition(), effElem, 15);
        particleSystem->AddFloatingText(target.GetPosition(), "-" + std::to_string(report.mitigatedDamage), themeCol, 26.0f);
    }

    std::string skillName = skill->GetName();
    if (Localization::IsKorean()) {
        if (skill->GetId() == "torrent_slash") skillName = "급류 베기";
        else if (skill->GetId() == "ignition_flask") skillName = "발화 플라스크";
        else if (skill->GetId() == "thunder_strike") skillName = "낙뢰 강타";
        else if (skill->GetId() == "glacial_lance") skillName = "빙하의 창";
        AddCombatLog("영웅이 [" + skillName + "]을(를) " + target.GetName() + "에게 시전하여 " + std::to_string(report.mitigatedDamage) + " 피해를 입혔습니다.", skill->GetThemeColor());
    } else {
        AddCombatLog("Player cast [" + skillName + "] on " + target.GetName() + " for " + std::to_string(report.mitigatedDamage) + " dmg.", skill->GetThemeColor());
    }

    if (report.reaction.triggered) {
        Color reactionColor = (Color){ 241, 196, 15, 255 };
        if (report.reaction.type == ReactionType::EXPLOSION) reactionColor = (Color){ 231, 76, 60, 255 };
        else if (report.reaction.type == ReactionType::FROZEN) reactionColor = (Color){ 162, 222, 255, 255 };
        else if (report.reaction.type == ReactionType::MELT) reactionColor = (Color){ 243, 156, 18, 255 };
        else if (report.reaction.type == ReactionType::PLASMA) reactionColor = (Color){ 155, 89, 182, 255 };

        if (particleSystem) {
            particleSystem->SpawnReactionBurst(target.GetPosition(), report.reaction.name, reactionColor);
            particleSystem->AddFloatingText(
                (Vector2){ target.GetX(), target.GetY() - 35.0f },
                "* " + report.reaction.name + " *",
                reactionColor,
                24.0f,
                1.6f
            );
        }

        std::string reactionName = report.reaction.name;
        if (Localization::IsKorean()) {
            if (report.reaction.type == ReactionType::SHOCK) reactionName = "감전 (SHOCK)";
            else if (report.reaction.type == ReactionType::EXPLOSION) reactionName = "폭발 (EXPLOSION)";
            else if (report.reaction.type == ReactionType::FROZEN) reactionName = "빙결 (FROZEN)";
            else if (report.reaction.type == ReactionType::MELT) reactionName = "증발/융해 (MELT)";
            else if (report.reaction.type == ReactionType::PLASMA) reactionName = "플라즈마 (PLASMA)";
            
            AddCombatLog("★ [원소 반응 발동: " + reactionName + "] " + report.reaction.description, ColorRGBA{ reactionColor.r, reactionColor.g, reactionColor.b, reactionColor.a });
            
            std::string bonusDetails = "-> 추가 반응 피해: +" + std::to_string(report.reaction.bonusDamage);
            if (report.reaction.chainAoE) {
                bonusDetails += " | 번개 아크가 모든 적에게 12 광역 전도 피해를 입힙니다!";
            }
            if (report.reaction.appliedElements != Element::NONE) {
                bonusDetails += " | [" + std::string(Localization::GetElementName(report.reaction.appliedElements)) + "] 부여 (" + std::to_string(report.reaction.appliedDuration) + "턴)";
            }
            if (report.reaction.stunTarget) {
                bonusDetails += " | 대상이 꽁꽁 얼어붙어 다음 턴 행동 불가!";
            }
            AddCombatLog(bonusDetails, ColorRGBA{ reactionColor.r, reactionColor.g, reactionColor.b, 220 });
        } else {
            AddCombatLog("* [REACTION TRIGGERED: " + report.reaction.name + "] " + report.reaction.description, ColorRGBA{ reactionColor.r, reactionColor.g, reactionColor.b, reactionColor.a });
            
            std::string bonusDetails = "-> Bonus Reaction Damage: +" + std::to_string(report.reaction.bonusDamage);
            if (report.reaction.chainAoE) {
                bonusDetails += " | Shockwave arcs 12 DMG across all enemies!";
            }
            if (report.reaction.appliedElements != Element::NONE) {
                bonusDetails += " | Inflicted [" + std::string(Localization::GetElementName(report.reaction.appliedElements)) + "] (" + std::to_string(report.reaction.appliedDuration) + "T)";
            }
            if (report.reaction.stunTarget) {
                bonusDetails += " | Target frozen solid (skips next action)!";
            }
            AddCombatLog(bonusDetails, ColorRGBA{ reactionColor.r, reactionColor.g, reactionColor.b, 220 });
        }

        if (report.reaction.chainAoE) {
            for (size_t i = 0; i < enemies.size(); ++i) {
                if (static_cast<int>(i) != targetIdx && enemies[i].IsAlive()) {
                    DamageReport chainReport = enemies[i].ApplyIncomingDamage(report.reaction.aoeDamage, Element::LIGHTNING);
                    if (particleSystem) {
                        particleSystem->SpawnHitSparks(enemies[i].GetPosition(), Element::LIGHTNING, 10);
                        particleSystem->AddFloatingText(enemies[i].GetPosition(), "-" + std::to_string(chainReport.mitigatedDamage) + " ARC", (Color){ 241, 196, 15, 255 });
                    }
                    if (Localization::IsKorean()) {
                        AddCombatLog("   -> 전도 아크가 " + enemies[i].GetName() + "에게 " + std::to_string(chainReport.mitigatedDamage) + " 피해를 입혔습니다!", ColorRGBA{ 241, 196, 15, 255 });
                    } else {
                        AddCombatLog("   -> Arc struck " + enemies[i].GetName() + " for " + std::to_string(chainReport.mitigatedDamage) + " dmg!", ColorRGBA{ 241, 196, 15, 255 });
                    }
                }
            }
        }
    }

    // Rune Special: Chain AoE on hit
    if (skill->HasChainAoE() && !report.reaction.chainAoE) {
        for (size_t i = 0; i < enemies.size(); ++i) {
            if (static_cast<int>(i) != targetIdx && enemies[i].IsAlive()) {
                DamageReport chainReport = enemies[i].ApplyIncomingDamage(10, Element::LIGHTNING);
                if (particleSystem) {
                    particleSystem->SpawnHitSparks(enemies[i].GetPosition(), Element::LIGHTNING, 10);
                    particleSystem->AddFloatingText(enemies[i].GetPosition(), "-" + std::to_string(chainReport.mitigatedDamage) + " ARC", (Color){ 241, 196, 15, 255 });
                }
                if (Localization::IsKorean()) {
                    AddCombatLog("   -> [연쇄 전도체] 아크가 " + enemies[i].GetName() + "에게 " + std::to_string(chainReport.mitigatedDamage) + " 피해를 입혔습니다!", ColorRGBA{ 241, 196, 15, 255 });
                } else {
                    AddCombatLog("   -> [Chain Conductor] Arc struck " + enemies[i].GetName() + " for " + std::to_string(chainReport.mitigatedDamage) + " dmg!", ColorRGBA{ 241, 196, 15, 255 });
                }
            }
        }
    }

    if (report.causedDeath) {
        if (Localization::IsKorean()) AddCombatLog(target.GetName() + " 처치 완료!", ColorRGBA{ 46, 204, 113, 255 });
        else AddCombatLog(target.GetName() + " was defeated!", ColorRGBA{ 46, 204, 113, 255 });
        
        if (particleSystem) particleSystem->AddFloatingText(target.GetPosition(), Localization::IsKorean() ? "처치!" : "DEFEATED", (Color){ 231, 76, 60, 255 }, 28.0f);
    }
}

// 2. Resolve Enemy AI Action Subroutine
void CombatSystem::ResolveEnemyAction(Enemy& enemy, Player& targetPlayer) {
    const Intent& intent = enemy.GetIntent();

    if (intent.type == IntentType::ATTACK) {
        DamageReport pReport = targetPlayer.ApplyIncomingDamage(intent.value, intent.element, targetPlayer.GetStance());

        if (particleSystem) {
            particleSystem->SpawnSlashEffect(enemy.GetPosition(), targetPlayer.GetPosition(), ToRaylibColor(enemy.GetColor()));
            particleSystem->SpawnHitSparks(targetPlayer.GetPosition(), intent.element, 10);
        }

        if (pReport.wasParried) {
            int counterDmg = 12;
            DamageReport counterReport = enemy.ApplyIncomingDamage(counterDmg, intent.element);
            if (intent.element != Element::NONE) {
                enemy.ApplyElement(intent.element, 2);
            }
            if (particleSystem) {
                particleSystem->AddFloatingText(targetPlayer.GetPosition(), Localization::IsKorean() ? "패링! -50%" : "PARRIED! -50%", (Color){ 241, 196, 15, 255 });
                particleSystem->AddFloatingText(enemy.GetPosition(), "-" + std::to_string(counterReport.mitigatedDamage) + (Localization::IsKorean() ? " 반격" : " COUNTER"), (Color){ 241, 196, 15, 255 });
            }
            if (Localization::IsKorean()) {
                AddCombatLog("⚔️ [패링 성공!] " + enemy.GetName() + "의 공격을 튕겨내고 " + std::to_string(counterReport.mitigatedDamage) + " 카운터 피해를 입혔습니다!", ColorRGBA{ 241, 196, 15, 255 });
            } else {
                AddCombatLog("[PARRY] Player deflected " + enemy.GetName() + "'s attack and countered for " + std::to_string(counterReport.mitigatedDamage) + " dmg!", ColorRGBA{ 241, 196, 15, 255 });
            }
        } else if (pReport.shieldAbsorbed > 0) {
            if (particleSystem) {
                particleSystem->AddFloatingText(targetPlayer.GetPosition(), Localization::IsKorean() ? ("방어 (" + std::to_string(pReport.shieldAbsorbed) + ")") : ("SHIELDED (" + std::to_string(pReport.shieldAbsorbed) + ")"), (Color){ 52, 152, 219, 255 });
                if (pReport.healthDamage > 0) {
                    particleSystem->AddFloatingText(targetPlayer.GetPosition(), "-" + std::to_string(pReport.healthDamage), (Color){ 231, 76, 60, 255 });
                }
            }
            if (Localization::IsKorean()) {
                AddCombatLog(enemy.GetName() + "의 공격: " + std::to_string(pReport.mitigatedDamage) + " 피해 (방어막이 " + std::to_string(pReport.shieldAbsorbed) + " 흡수).", ColorRGBA{ 231, 76, 60, 255 });
            } else {
                AddCombatLog(enemy.GetName() + " attacks for " + std::to_string(pReport.mitigatedDamage) + " dmg (Shield absorbed " + std::to_string(pReport.shieldAbsorbed) + ").", ColorRGBA{ 231, 76, 60, 255 });
            }
        } else {
            if (particleSystem) particleSystem->AddFloatingText(targetPlayer.GetPosition(), "-" + std::to_string(pReport.healthDamage), (Color){ 231, 76, 60, 255 });
            if (Localization::IsKorean()) {
                AddCombatLog(enemy.GetName() + "이(가) 영웅을 타격하여 " + std::to_string(pReport.healthDamage) + " 피해를 입혔습니다.", ColorRGBA{ 231, 76, 60, 255 });
            } else {
                AddCombatLog(enemy.GetName() + " strikes Player for " + std::to_string(pReport.healthDamage) + " dmg.", ColorRGBA{ 231, 76, 60, 255 });
            }
        }
    } else if (intent.type == IntentType::DEFEND) {
        enemy.AddShield(intent.value);
        if (particleSystem) particleSystem->AddFloatingText(enemy.GetPosition(), "+" + std::to_string(intent.value) + (Localization::IsKorean() ? " 방어막" : " SHIELD"), (Color){ 52, 152, 219, 255 });
        if (Localization::IsKorean()) {
            AddCombatLog(enemy.GetName() + "이(가) [" + intent.name + "]을(를) 시전하여 " + std::to_string(intent.value) + " 방어막을 얻었습니다.", ColorRGBA{ 52, 152, 219, 255 });
        } else {
            AddCombatLog(enemy.GetName() + " casts [" + intent.name + "] gaining " + std::to_string(intent.value) + " Shield.", ColorRGBA{ 52, 152, 219, 255 });
        }
    } else if (intent.type == IntentType::DEBUFF) {
        if (targetPlayer.GetStance() == StanceType::PARRY) {
            enemy.ApplyElement(intent.element, 2);
            if (particleSystem) particleSystem->AddFloatingText(targetPlayer.GetPosition(), Localization::IsKorean() ? "디버프 반사!" : "PARRIED DEBUFF!", (Color){ 241, 196, 15, 255 });
            if (Localization::IsKorean()) {
                AddCombatLog("⚔️ [패링 성공!] " + enemy.GetName() + "의 [" + intent.name + "]을(를) 반사하여 역으로 부여했습니다!", ColorRGBA{ 241, 196, 15, 255 });
            } else {
                AddCombatLog("[PARRY] Player reflected [" + intent.name + "] back to " + enemy.GetName() + "!", ColorRGBA{ 241, 196, 15, 255 });
            }
        } else {
            targetPlayer.ApplyElement(intent.element, 2);
            if (particleSystem) particleSystem->AddFloatingText(targetPlayer.GetPosition(), "+" + std::string(Localization::GetElementName(intent.element)), ToRaylibColor(ElementalSystem::GetElementColor(intent.element)));
            if (Localization::IsKorean()) {
                AddCombatLog(enemy.GetName() + "이(가) [" + intent.name + "]을(를) 시전하여 영웅에게 [" + Localization::GetElementName(intent.element) + "]을(를) 부여했습니다.", ElementalSystem::GetElementColor(intent.element));
            } else {
                AddCombatLog(enemy.GetName() + " casts [" + intent.name + "] inflicting [" + Localization::GetElementName(intent.element) + "] on Player.", ElementalSystem::GetElementColor(intent.element));
            }
        }
    } else if (intent.type == IntentType::BUFF) {
        enemy.Heal(intent.value);
        if (particleSystem) particleSystem->AddFloatingText(enemy.GetPosition(), "+" + std::to_string(intent.value) + " HP", (Color){ 46, 204, 113, 255 });
        if (Localization::IsKorean()) {
            AddCombatLog(enemy.GetName() + "이(가) [" + intent.name + "]을(를) 사용하여 " + std::to_string(intent.value) + " 체력을 회복했습니다.", ColorRGBA{ 46, 204, 113, 255 });
        } else {
            AddCombatLog(enemy.GetName() + " channels [" + intent.name + "] restoring " + std::to_string(intent.value) + " HP.", ColorRGBA{ 46, 204, 113, 255 });
        }
    }
}

// 3. Apply Active Weather Environmental Effect Subroutine
void CombatSystem::ApplyActiveWeatherEffect() {
    WeatherTriggerResult weatherRes = weatherSystem.ProcessTurnStartWeather();

    if (Localization::IsKorean()) {
        AddCombatLog("[날씨 환경 활성화: " + std::string(Localization::GetWeatherName(weatherSystem.GetCurrentWeather())) + "] " + Localization::GetWeatherShortDesc(weatherSystem.GetCurrentWeather()), weatherRes.weatherColor);
    } else {
        AddCombatLog("[WEATHER ACTIVATION: " + weatherRes.title + "] " + weatherRes.description, weatherRes.weatherColor);
    }

    if (weatherRes.globalStatusToApply != Element::NONE) {
        player.ApplyElement(weatherRes.globalStatusToApply, weatherRes.statusDuration);
        if (particleSystem) particleSystem->AddFloatingText(player.GetPosition(), "+" + std::string(Localization::GetElementName(weatherRes.globalStatusToApply)), ToRaylibColor(weatherRes.weatherColor));

        for (auto& enemy : enemies) {
            if (enemy.IsAlive()) {
                if (weatherSystem.GetCurrentWeather() == WeatherType::BLIZZARD && enemy.HasElement(Element::WET)) {
                    enemy.ClearElement(Element::WET);
                    enemy.SetFrozen(true);
                    if (particleSystem) particleSystem->AddFloatingText(enemy.GetPosition(), Localization::IsKorean() ? "빙결!" : "[FROZEN]!", (Color){ 162, 222, 255, 255 }, 24.0f);
                    if (Localization::IsKorean()) {
                        AddCombatLog(enemy.GetName() + "이(가) 눈보라에 수분이 얼어붙어 [빙결] 상태가 되었습니다!", ColorRGBA{ 162, 222, 255, 255 });
                    } else {
                        AddCombatLog(enemy.GetName() + " is drenched and freezes solid in the blizzard!", ColorRGBA{ 162, 222, 255, 255 });
                    }
                } else {
                    enemy.ApplyElement(weatherRes.globalStatusToApply, weatherRes.statusDuration);
                    if (particleSystem) particleSystem->AddFloatingText(enemy.GetPosition(), "+" + std::string(Localization::GetElementName(weatherRes.globalStatusToApply)), ToRaylibColor(weatherRes.weatherColor));
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
                particleSystem->AddFloatingText(enemies[targetIdx].GetPosition(), "-" + std::to_string(strike.mitigatedDamage) + (Localization::IsKorean() ? " 낙뢰" : " LIGHTNING"), (Color){ 241, 196, 15, 255 });
            }
            if (Localization::IsKorean()) {
                AddCombatLog("⚡ 뇌우의 낙뢰가 " + enemies[targetIdx].GetName() + "에게 떨어져 " + std::to_string(strike.mitigatedDamage) + " 피해를 입혔습니다!", ColorRGBA{ 241, 196, 15, 255 });
            } else {
                AddCombatLog("Lightning bolt strikes " + enemies[targetIdx].GetName() + " for " + std::to_string(strike.mitigatedDamage) + " dmg!", ColorRGBA{ 241, 196, 15, 255 });
            }
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
            if (Localization::IsKorean()) AddCombatLog("🌪️ 돌풍이 모든 적의 상태이상을 전장 전체로 확산 전파했습니다!", ColorRGBA{ 46, 204, 113, 255 });
            else AddCombatLog("Gale winds swirl active statuses across all combatants!", ColorRGBA{ 46, 204, 113, 255 });
        }
    }
}

// Complete Sequential Turn Execution Pipeline
void CombatSystem::ExecuteTurn() {
    if (Localization::IsKorean()) {
        AddCombatLog("=== 제 " + std::to_string(turnCounter) + " 턴 진행 ===", ColorRGBA{ 241, 196, 15, 255 });
    } else {
        AddCombatLog("=== ROUND " + std::to_string(turnCounter) + " RESOLUTION ===", ColorRGBA{ 241, 196, 15, 255 });
    }

    // 1. Resolve Player Action
    ResolvePlayerAction(selectedSkillIndex, selectedTargetIndex, selectedStance);

    // 2. Check if all enemies are dead (Wave clear / Victory check)
    if (CheckWaveCleared()) {
        currentPhase = CombatPhase::VICTORY_SCREEN;
        if (Localization::IsKorean()) AddCombatLog("=== 승리: 모든 적을 격파했습니다! ===", ColorRGBA{ 46, 204, 113, 255 });
        else AddCombatLog("=== VICTORY: Wave Cleared! ===", ColorRGBA{ 46, 204, 113, 255 });
        return;
    }

    // 3. Resolve Enemy AI Actions
    for (auto& enemy : enemies) {
        if (!enemy.IsAlive()) continue;

        if (enemy.IsFrozen()) {
            enemy.SetFrozen(false);
            if (particleSystem) particleSystem->AddFloatingText(enemy.GetPosition(), Localization::IsKorean() ? "빙결 해제 (행동 스킵)" : "THAWED (Turn Skipped)", (Color){ 162, 222, 255, 255 });
            if (Localization::IsKorean()) AddCombatLog(enemy.GetName() + "이(가) [빙결] 상태여서 이번 턴 행동을 스킵했습니다!", ColorRGBA{ 162, 222, 255, 255 });
            else AddCombatLog(enemy.GetName() + " is [FROZEN] and skips their action!", ColorRGBA{ 162, 222, 255, 255 });
        } else {
            ResolveEnemyAction(enemy, player);
        }
        // Roll next turn's intent for the enemy
        enemy.AdvancePattern();
    }

    // 4. Check if player died from enemy attacks
    if (!player.IsAlive()) {
        currentPhase = CombatPhase::DEFEAT_SCREEN;
        if (Localization::IsKorean()) AddCombatLog("=== 패배: 영웅이 전투에서 쓰러졌습니다 ===", ColorRGBA{ 231, 76, 60, 255 });
        else AddCombatLog("=== DEFEAT: You have fallen in combat ===", ColorRGBA{ 231, 76, 60, 255 });
        return;
    }

    // 5. Tick Status Effects (Buffs, Debuffs, DoTs) on Player and all Enemies
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

    // Check if wave cleared or player died from DoTs
    if (CheckWaveCleared()) {
        currentPhase = CombatPhase::VICTORY_SCREEN;
        if (Localization::IsKorean()) AddCombatLog("=== 승리: 모든 적을 격파했습니다! ===", ColorRGBA{ 46, 204, 113, 255 });
        else AddCombatLog("=== VICTORY: Wave Cleared! ===", ColorRGBA{ 46, 204, 113, 255 });
        return;
    }
    if (!player.IsAlive()) {
        currentPhase = CombatPhase::DEFEAT_SCREEN;
        if (Localization::IsKorean()) AddCombatLog("=== 패배: 영웅이 전투에서 쓰러졌습니다 ===", ColorRGBA{ 231, 76, 60, 255 });
        else AddCombatLog("=== DEFEAT: You have fallen in combat ===", ColorRGBA{ 231, 76, 60, 255 });
        return;
    }

    // 6. Decrement Skill Cooldowns
    player.TickCooldowns(); // for each skill: if (cooldown > 0) cooldown--;
    if (Localization::IsKorean()) AddCombatLog("스킬 쿨다운이 1턴 감소했습니다.", ColorRGBA{ 160, 175, 200, 255 });
    else AddCombatLog("Player skill cooldowns updated (-1 Turn).", ColorRGBA{ 160, 175, 200, 255 });

    // 7. Advance Weather Forecast Queue & Apply
    weatherSystem.AdvanceTurn();
    ApplyActiveWeatherEffect();

    // 8. Reset Turn State
    turnCounter++;
    player.ResetShield();
    for (auto& enemy : enemies) {
        enemy.ResetShield();
        if (enemy.IsAlive()) {
            enemy.DecideIntent(turnCounter, player, weatherSystem.GetCurrentWeather());
        }
    }

    // Retarget if current target is dead
    if (selectedTargetIndex < 0 || selectedTargetIndex >= static_cast<int>(enemies.size()) || !enemies[selectedTargetIndex].IsAlive()) {
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
        return;
    }

    if (CheckWaveCleared()) {
        currentPhase = CombatPhase::VICTORY_SCREEN;
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
}
