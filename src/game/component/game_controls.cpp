#include "game_controls.hpp"
#include <raygui.h>
#include "../game_globals.hpp"

constexpr const int STROKE_WIDTH = 2;
constexpr const int CONTROLS_TEXT_SIZE = 18;

GameControls::GameControls() :
	bigBoxesInputBox({ 290, 85, 130, 25 }, CONTROLS_TEXT_SIZE, 2, 1, 4),
	smallBoxesInputBox({ 290, 115, 130, 25 }, CONTROLS_TEXT_SIZE, 4, 2, 8),
	auxiliarDocksInputBox({ 290, 145, 130, 25 }, CONTROLS_TEXT_SIZE, 1, 1, 4) {
	this->smallBoxesInputBox.mustBeEven = true;
}

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

	DrawTextEx(controlsFont, "Ingrese la cantidad de cajas grandes: ", { 10, 85 + 3 }, CONTROLS_TEXT_SIZE, 0, WHITE);
	this->bigBoxesInputBox.draw();

	DrawTextEx(controlsFont, "Ingrese la cantidad de cajas pequeñas: ", { 10, 115 + 3 }, CONTROLS_TEXT_SIZE, 0, WHITE);
	this->smallBoxesInputBox.draw();

	DrawTextEx(controlsFont, "Ingrese la cantidad de muelles auxiliares: ", { 10, 145 + 3 }, CONTROLS_TEXT_SIZE, 0, WHITE);
	this->auxiliarDocksInputBox.draw();

	bool canSimulate = this->bigBoxesInputBox.currentValueValid &&
		this->smallBoxesInputBox.currentValueValid &&
		this->auxiliarDocksInputBox.currentValueValid &&
		!simulation->playingBeginAnimation;

	if (!canSimulate) GuiDisable();

	GuiSetFont(controlsFont);
	GuiSetStyle(DEFAULT, TEXT_SIZE, CONTROLS_TEXT_SIZE);
	if (GuiButton({ 10, 180, 170, 30 }, "Empezar juego")) {
		simulation->begin(this->bigBoxesInputBox.value, this->smallBoxesInputBox.value / 2, this->auxiliarDocksInputBox.value);
	}
	if (GuiButton({ 190, 180, 170, 30 }, "Empezar simulación")) {
		simulation->beginAndSolve(this->bigBoxesInputBox.value, this->smallBoxesInputBox.value / 2, this->auxiliarDocksInputBox.value);
	}

	if (!canSimulate) GuiEnable();

	DrawRectangle(0, 220, renderWidth, STROKE_WIDTH, WHITE);
}

void GameControls::unload() {
	this->bigBoxesInputBox.unload();
	this->smallBoxesInputBox.unload();
	this->auxiliarDocksInputBox.unload();
	if (this->controlsFont.has_value()) {
		UnloadFont(*this->controlsFont);
	}
}
