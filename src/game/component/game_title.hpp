#pragma once

#include <raylib.h>
#include <optional>

class GameTitle {
private:
	std::optional<Font> mainFont;
	std::optional<Font> secondaryFont;
	Font getMainFont();
	Font getSecondaryFont();
public:
	GameTitle();
	void draw();
	void unload();
};