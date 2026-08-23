#pragma once

#include "Core/Types.hpp"
#include <string>

enum class Language {
    KOREAN = 0,
    ENGLISH = 1
};

class Localization {
private:
    static Language currentLanguage;

public:
    static void SetLanguage(Language lang) { currentLanguage = lang; }
    static Language GetLanguage() { return currentLanguage; }
    static bool IsKorean() { return currentLanguage == Language::KOREAN; }
    static void ToggleLanguage() {
        currentLanguage = (currentLanguage == Language::KOREAN) ? Language::ENGLISH : Language::KOREAN;
    }

    // Element Strings
    static const char* GetElementName(Element elem) {
        if (currentLanguage == Language::KOREAN) {
            switch (elem) {
                case Element::WET:       return "수분(Wet)";
                case Element::FIRE:      return "화염(Fire)";
                case Element::OIL:       return "기름(Oil)";
                case Element::LIGHTNING: return "전기(Lightning)";
                case Element::COLD:      return "냉기(Cold)";
                case Element::GALE:      return "돌풍(Gale)";
                case Element::NONE:
                default:                 return "무속성";
            }
        } else {
            switch (elem) {
                case Element::WET:       return "Wet";
                case Element::FIRE:      return "Fire";
                case Element::OIL:       return "Oil";
                case Element::LIGHTNING: return "Lightning";
                case Element::COLD:      return "Cold";
                case Element::GALE:      return "Gale";
                case Element::NONE:
                default:                 return "None";
            }
        }
    }

    static const char* GetElementTag(Element elem) {
        if (currentLanguage == Language::KOREAN) {
            switch (elem) {
                case Element::WET:       return "[수분]";
                case Element::FIRE:      return "[화염]";
                case Element::OIL:       return "[기름]";
                case Element::LIGHTNING: return "[전기]";
                case Element::COLD:      return "[냉기]";
                case Element::GALE:      return "[돌풍]";
                case Element::NONE:
                default:                 return "[무속성]";
            }
        } else {
            switch (elem) {
                case Element::WET:       return "[WET]";
                case Element::FIRE:      return "[FIRE]";
                case Element::OIL:       return "[OIL]";
                case Element::LIGHTNING: return "[ELEC]";
                case Element::COLD:      return "[COLD]";
                case Element::GALE:      return "[GALE]";
                case Element::NONE:
                default:                 return "[NONE]";
            }
        }
    }

    // Weather Strings
    static const char* GetWeatherName(WeatherType weather) {
        if (currentLanguage == Language::KOREAN) {
            switch (weather) {
                case WeatherType::RAIN:         return "폭우 (Rain)";
                case WeatherType::HEATWAVE:     return "폭염 (Heatwave)";
                case WeatherType::STORM:        return "뇌우 (Storm)";
                case WeatherType::BLIZZARD:     return "눈보라 (Blizzard)";
                case WeatherType::GALE:         return "돌풍 (Gale)";
                case WeatherType::ACID_RAIN:    return "산성비 (Acid Rain)";
                case WeatherType::CLEAR:
                default:                        return "맑음 (Clear)";
            }
        } else {
            switch (weather) {
                case WeatherType::RAIN:         return "Rain";
                case WeatherType::HEATWAVE:     return "Heatwave";
                case WeatherType::STORM:        return "Storm";
                case WeatherType::BLIZZARD:     return "Blizzard";
                case WeatherType::GALE:         return "Gale";
                case WeatherType::ACID_RAIN:    return "Acid Rain";
                case WeatherType::CLEAR:
                default:                        return "Clear";
            }
        }
    }

    static const char* GetWeatherShortDesc(WeatherType weather) {
        if (currentLanguage == Language::KOREAN) {
            switch (weather) {
                case WeatherType::RAIN:         return "매턴 [수분] 부여 / 물 속성 +35%";
                case WeatherType::HEATWAVE:     return "화염 피해 +50% 증폭";
                case WeatherType::STORM:        return "[수분] 부여 + 15 낙뢰 강타";
                case WeatherType::BLIZZARD:     return "[수분] 대상 즉시 빙결 / 냉기 +30%";
                case WeatherType::GALE:         return "모든 적에게 상태이상 확산 전파";
                case WeatherType::ACID_RAIN:    return "가연성 [기름] 코팅";
                case WeatherType::CLEAR:
                default:                        return "표준 환경 (추가 효과 없음)";
            }
        } else {
            switch (weather) {
                case WeatherType::RAIN:         return "Applies [WET] / Boosts Water +35%";
                case WeatherType::HEATWAVE:     return "+50% Fire DMG bonus";
                case WeatherType::STORM:        return "[WET] + 15 Lightning strike";
                case WeatherType::BLIZZARD:     return "Applies [COLD] / Freezes Wet targets";
                case WeatherType::GALE:         return "Spreads status debuffs across all units";
                case WeatherType::ACID_RAIN:    return "Coats all units in [OIL]";
                case WeatherType::CLEAR:
                default:                        return "Standard combat conditions";
            }
        }
    }

    // Stance Strings
    static const char* GetStanceName(StanceType stance) {
        if (currentLanguage == Language::KOREAN) {
            switch (stance) {
                case StanceType::ATTACK:  return "공격 태세 [Q] (+40% 공격력)";
                case StanceType::DEFENSE: return "방어 태세 [W] (+18 방어막, -30% 피해)";
                case StanceType::PARRY:   return "패링 태세 [E] (반격 & 상태이상 반사)";
            }
        } else {
            switch (stance) {
                case StanceType::ATTACK:  return "[ATK] ATTACK STANCE (+40% DMG)";
                case StanceType::DEFENSE: return "[DEF] DEFENSE (+18 Shield, -30% DMG)";
                case StanceType::PARRY:   return "[PARRY] PARRY (Counter & Reflect)";
            }
        }
        return "";
    }
};
