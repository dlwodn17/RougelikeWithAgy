# RougelikeWithAgy (Elemental Reaction x Weather Forecast Roguelike)

A tactical 2D turn-based roguelike combat prototype built in **C++17** and **Raylib**, featuring deep environmental interactions and synergistic elemental chain reactions.

---

## 🌟 Core Gameplay Mechanics

### 1. Weather Forecast System (1-3 Turns Ahead)
A dynamic environmental forecast queue displays upcoming weather patterns 1-3 turns in advance. At the start of each turn, the active weather shifts and applies global modifiers:
- 🌧️ **Downpour Rain**: Applies `[WET]` to all combatants every turn. Boosts Water damage (+35%) and dampens Fire damage.
- 🔥 **Scorching Heatwave**: Boosts all Fire damage by +50% and intensifies combustion.
- ⛈️ **Thunderstorm**: Applies `[WET]` globally and calls down random 15-damage lightning strikes.
- 🌨️ **Glacial Blizzard**: Applies `[COLD]` to all units. Any drenched (`[WET]`) unit instantly **Freezes solid**!
- 🌪️ **Howling Gale**: Spreads all active elemental debuffs across neighboring combatants.
- 🧪 **Corrosive Acid Rain**: Drenches everyone in reactive `[OIL]`, priming the battlefield for explosive combos.

---

### 2. Elemental Chain Reaction Matrix
Entities maintain an active status buffer. Applying complementary elements triggers powerful reactions:

| Reaction | Formula | Effect |
| :--- | :--- | :--- |
| **💥 SHOCK** | `WET + LIGHTNING` | Bonus lightning damage + **Arcs AoE damage** to all other enemies! |
| **💣 EXPLOSION** | `OIL + FIRE` | Massive burst damage + applies **Burning DoT** |
| **🧊 FROZEN** | `WET + COLD` | Flash-freezes target; **skips their next action/turn** |
| **💨 MELT / STEAM** | `FIRE + COLD` | High-temperature steam blast bonus damage |
| **⚡ PLASMA** | `OIL + LIGHTNING` | High-voltage piercing discharge |

---

### 3. Cooldown-based Skills & Stance Mastery
No random card draw or mana starvation—combat is driven purely by skill cooldown management and elemental timing:

#### ⚔️ Active Skills:
1. **Torrent Slash** (CD: 0 turns) — 14 Water DMG, inflicts `[WET]`. (Synergy Primer)
2. **Ignition Flask** (CD: 2 turns) — 10 Fire DMG, applies `[OIL]` + `[FIRE]` for immediate `[EXPLOSION]`.
3. **Thunder Strike** (CD: 2 turns) — 24 Lightning DMG, triggers `[SHOCK]` AoE chain on `[WET]` targets.
4. **Glacial Lance** (CD: 3 turns) — 18 Cold DMG, triggers `[FROZEN]` on `[WET]` targets to stun them.

#### 🛡️ Stance Selection:
- **⚔️ Attack Stance `[Q]`**: +40% Outgoing Damage.
- **🛡️ Defense Stance `[W]`**: +18 Shield, -30% Incoming Damage.
- **⚡ Parry Stance `[E]`**: -50% Incoming Damage; **Reflects incoming status debuffs** back to the attacker and deals counter damage!

---

## 🎮 Controls
- **1, 2, 3, 4**: Select Skill
- **Click Enemy**: Select Target
- **Q, W, E**: Select Stance (Attack / Defense / Parry)
- **SPACE / ENTER**: Execute Turn
- **H / TAB**: Toggle Reaction & Weather Guide
- **R**: Restart Run

---

## 🏗️ Architecture & Decoupled Design

```
RougelikeWithAgy/
├── CMakeLists.txt         # Cross-platform CMake with Raylib FetchContent
├── Makefile               # Optional native Makefile
├── include/
│   ├── Common.hpp          # Data models, Enums, Structs, Color helpers
│   ├── ElementalSystem.hpp # Elemental buffer, Reactions engine, Spread
│   ├── WeatherSystem.hpp   # 3-turn forecast queue, weather rules & triggers
│   ├── Entity.hpp          # Entity base, Player, Enemy AI & Intents
│   ├── SkillSystem.hpp     # 4 active cooldown-based skills
│   ├── ParticleSystem.hpp  # Weather ambient particles, reaction bursts, floating text
│   ├── UIRenderer.hpp      # 2D Raylib HUD, forecast ribbon, skill cards, combat log
│   ├── CombatSystem.hpp    # Turn-phase state machine, Parry logic, wave progression
│   └── GameState.hpp       # Game manager, scene state transitions
└── src/
    ├── main.cpp            # Entry point & Raylib window loop
    ├── Entity.cpp
    ├── ElementalSystem.cpp
    ├── WeatherSystem.cpp
    ├── SkillSystem.cpp
    ├── ParticleSystem.cpp
    ├── UIRenderer.cpp
    ├── CombatSystem.cpp
    └── GameState.cpp
```

---

## 🚀 How to Build & Run

### Prerequisites
- C++17 compatible compiler (GCC / Clang / MSVC)
- CMake 3.16+

### CMake Build (Recommended)
```bash
# 1. Create build directory
mkdir build
cd build

# 2. Configure with CMake (automatically fetches Raylib if not installed locally)
cmake ..

# 3. Compile
cmake --build . --config Release

# 4. Run executable
./RougelikeWithAgy
```

---

## 📜 License
MIT License. Built with ❤️ using Raylib and modern C++17.
