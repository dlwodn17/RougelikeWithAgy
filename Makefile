CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -Iinclude -Ithird_party/raylib/src
LDFLAGS = -L. -lraylib -lopengl32 -lgdi32 -lwinmm

SRC = src/main.cpp \
      src/Entity.cpp \
      src/ElementalSystem.cpp \
      src/WeatherSystem.cpp \
      src/SkillSystem.cpp \
      src/ParticleSystem.cpp \
      src/GameRenderer.cpp \
      src/UIRenderer.cpp \
      src/CombatSystem.cpp \
      src/GameState.cpp

TARGET = RougelikeWithAgy.exe

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET)
