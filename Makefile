CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -Iinclude -Ithird_party/raylib/src
LDFLAGS = -L. -lraylib -lopengl32 -lgdi32 -lwinmm

SRC = src/main.cpp \
      src/GameState.cpp \
      src/Entities/Entity.cpp \
      src/Entities/Player.cpp \
      src/Entities/Enemy.cpp \
      src/Entities/Skill.cpp \
      src/Systems/ElementalSystem.cpp \
      src/Systems/WeatherSystem.cpp \
      src/Systems/CombatSystem.cpp \
      src/Core/Localization.cpp \
      src/Renderer/ParticleSystem.cpp \
      src/Renderer/FontManager.cpp \
      src/Renderer/WeatherRenderer.cpp \
      src/Renderer/CombatRenderer.cpp \
      src/Renderer/UIRenderer.cpp

TARGET = RougelikeWithAgy.exe

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET)
