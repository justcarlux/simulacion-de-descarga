#pragma once

#include <raylib.h>
#include <optional>
#include <utility>

constexpr const int MAX_INPUT_CHARACTERS = 12;

class IntegerInputBox {
private:
	std::optional<Font> font;
	std::optional<std::pair<int, int>> range;
	Rectangle bounds;
	int textSize;
	int currentIndex;
	char textBuffer[MAX_INPUT_CHARACTERS + 1];
	bool selected;
	void setupInitialValue();
	Font getFont();
	bool textBufferContainsDot();
	bool textBufferIsEmpty();
public:
	int value;
	bool currentValueValid;
	bool mustBeEven = false;
	IntegerInputBox(Rectangle bounds, int textSize, int value);
	IntegerInputBox(Rectangle bounds, int textSize, int value, int min, int max);
	void draw();
	void unload();
};