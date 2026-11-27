#include "game_status_bar.hpp"
#include "../game_globals.hpp"
#include "raygui.h"

constexpr const int STROKE_WIDTH = 2;
constexpr const int STATUS_TEXT_SIZE = 18;
constexpr const int PADDING_X = 12;
constexpr const int ITEM_GAP = 8;
constexpr const int BUTTON_SIZE = 24;
constexpr const Color STATUS_BAR_COLOR = { 35, 35, 35, 255 };

GameStatusBar::GameStatusBar() {}

Font GameStatusBar::getFont() {
	if (!this->font.has_value()) {
		this->font = LoadFontFromMemory(".ttf", GameFonts::UBUNTU_LIGHT->data, GameFonts::UBUNTU_LIGHT->size, STATUS_TEXT_SIZE, nullptr, 250);
	}
	return *this->font;
}

void GameStatusBar::draw(GameSimulation& simulation) {
	int renderWidth = GetRenderWidth();
	int top = GetRenderHeight() - GameLayout::STATUS_BAR_HEIGHT;

	DrawRectangle(0, top, renderWidth, GameLayout::STATUS_BAR_HEIGHT, STATUS_BAR_COLOR);
	DrawRectangle(0, top, renderWidth, STROKE_WIDTH, WHITE);

	Font font = this->getFont();

	float textY = top + (GameLayout::STATUS_BAR_HEIGHT - STATUS_TEXT_SIZE) / 2.0f;
	DrawTextEx(font, this->getMainText(simulation), { 10, textY + 1 }, STATUS_TEXT_SIZE, 0, WHITE);

	const char* speedText = TextFormat("Velocidad de reproducción de soluciones: %.1fx", simulation.solutionPlaybackSpeed);
	Vector2 speedSize = MeasureTextEx(font, speedText, STATUS_TEXT_SIZE, 0);

	float controlY = (top + (GameLayout::STATUS_BAR_HEIGHT - BUTTON_SIZE) / 2.0f); 
	float speedTextX = renderWidth - PADDING_X - speedSize.x;
	float plusBtnX = speedTextX - ITEM_GAP - BUTTON_SIZE;
	float minusBtnX = plusBtnX - ITEM_GAP - BUTTON_SIZE;

	if (GuiButton({ minusBtnX, controlY, (float)BUTTON_SIZE, (float)BUTTON_SIZE + 1 }, "-")) {
		simulation.decreaseSolutionSpeed();
	}
	if (GuiButton({ plusBtnX, controlY, (float)BUTTON_SIZE, (float)BUTTON_SIZE + 1 }, "+")) {
		simulation.increaseSolutionSpeed();
	}
	DrawTextEx(font, speedText, { renderWidth - speedSize.x - PADDING_X, textY + 1 }, STATUS_TEXT_SIZE, 0, WHITE);
}

const char* GameStatusBar::getMainText(GameSimulation& simulation) {
	if (!simulation.simulatedAtLeastOnce) {
		return "Estado: Nada por el momento.";
	}
	else if (simulation.playingBeginAnimation) {
		return TextFormat(
			"Estado: Cargando muelle origen... | Grandes: %d | Pequeñas: %d | Auxiliares: %d",
			simulation.bigBoxes,
			simulation.smallGroups * 2,
			simulation.auxiliarDocks
		);
	}
	else if (simulation.playingSolution) {
		return TextFormat(
			"Estado: Reproduciendo solución... | Grandes: %d | Pequeñas: %d | Auxiliares: %d",
			simulation.bigBoxes,
			simulation.smallGroups * 2,
			simulation.auxiliarDocks
		);
	}
	else if (simulation.areAllBoxesInDestination()) {
		return TextFormat(
			"Estado: ¡Simulación completada! Puedes iniciar otra. | Grandes: %d | Pequeñas: %d | Auxiliares: %d",
			simulation.bigBoxes,
			simulation.smallGroups * 2,
			simulation.auxiliarDocks
		);
	}
	else {
		return TextFormat(
			"Estado: Jugando. Puedes mover las cajas con el ratón. | Grandes: %d | Pequeñas: %d | Auxiliares: %d",
			simulation.bigBoxes,
			simulation.smallGroups * 2,
			simulation.auxiliarDocks
		);
	}
}

void GameStatusBar::unload() {
	if (this->font.has_value()) {
		UnloadFont(*this->font);
	}
}
