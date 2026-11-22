#pragma once

#include <raylib.h>
#include <optional>
#include "game_simulation.hpp"

class GameStatusBar {
private:
	std::optional<Font> font;
	Font getFont();
	const char* getMainText(GameSimulation& simulation);
public:
	GameStatusBar();
	void draw(GameSimulation& simulation);
	void unload();
};
