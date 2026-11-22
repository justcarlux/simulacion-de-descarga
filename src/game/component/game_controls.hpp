#pragma once

#include <raylib.h>
#include <optional>
#include "integer_input_box.hpp"
#include "game_simulation.hpp"

class GameControls {
private:
	std::optional<Font> controlsFont;
	Font getControlsFont();
public:
	IntegerInputBox boxesInputBox;
	IntegerInputBox auxiliarDocksInputBox;
	GameControls();
	void draw(GameSimulation* simulation);
	void unload();
};
