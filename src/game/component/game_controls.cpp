#include "game_controls.hpp"
#include <raygui.h>
#include "../game_globals.hpp"

constexpr const int STROKE_WIDTH = 2;
constexpr const int CONTROLS_TEXT_SIZE = 18;

GameControls::GameControls() :
	boxesInputBox({ 290, 85, 130, 25 }, CONTROLS_TEXT_SIZE, 2, 2, 5),
	auxiliarDocksInputBox({ 290, 115, 130, 25 }, CONTROLS_TEXT_SIZE, 1, 1, 4) {}

Font GameControls::getControlsFont() {
	if (!this->controlsFont.has_value()) {
		this->controlsFont = LoadFontFromMemory(".ttf", GameFonts::UBUNTU_LIGHT->data, GameFonts::UBUNTU_LIGHT->size, CONTROLS_TEXT_SIZE, nullptr, 250);
	}
	return *this->controlsFont;
}

void GameControls::draw(GameSimulation* simulation) {
	int renderWidth = GetRenderWidth();
	Font controlsFont = this->getControlsFont();

	DrawRectangle(0, 75, renderWidth, STROKE_WIDTH, WHITE);

	DrawTextEx(controlsFont, "Ingrese la cantidad de cajas: ", { 10, 85 + 3 }, CONTROLS_TEXT_SIZE, 0, WHITE);
	this->boxesInputBox.draw();

	DrawTextEx(controlsFont, "Ingrese la cantidad de muelles auxiliares: ", { 10, 115 + 3 }, CONTROLS_TEXT_SIZE, 0, WHITE);
	this->auxiliarDocksInputBox.draw();

	bool canSimulate = this->boxesInputBox.currentValueValid &&
		this->auxiliarDocksInputBox.currentValueValid &&
		!simulation->playingBeginAnimation;

	if (!canSimulate) GuiDisable();

	GuiSetFont(controlsFont);
	GuiSetStyle(DEFAULT, TEXT_SIZE, CONTROLS_TEXT_SIZE);
	if (GuiButton({ 10, 150, 170, 30 }, "Empezar simulación")) {
		simulation->begin(this->boxesInputBox.value, this->auxiliarDocksInputBox.value);
	}
	if (GuiButton({ 190, 150, 300, 30 }, "Empezar y ver solución de la simulación")) {
		simulation->beginAndSolve(this->boxesInputBox.value, this->auxiliarDocksInputBox.value);
	}

	if (!canSimulate) GuiEnable();

	DrawRectangle(0, 190, renderWidth, STROKE_WIDTH, WHITE);
}

void GameControls::unload() {
	this->boxesInputBox.unload();
	this->auxiliarDocksInputBox.unload();
	if (this->controlsFont.has_value()) {
		UnloadFont(*this->controlsFont);
	}
}
