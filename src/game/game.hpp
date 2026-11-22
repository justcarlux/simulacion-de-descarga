#pragma once

#pragma once
#include "./component/game_title.hpp";
#include "./component/game_controls.hpp";
#include "./component/game_simulation.hpp";
#include "./component/game_status_bar.hpp";

class Game {
private:
	GameTitle gameTitle;
	GameControls gameControls;
	GameSimulation gameSimulation;
	GameStatusBar gameStatusBar;
	void draw();
	void update();
public:
	Game();
	void run();
	void unload();
};