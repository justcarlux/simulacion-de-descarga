#include "game_simulation.hpp"
#include <algorithm>
#include "../game_globals.hpp"
#include "../../frame_stewart_solver.h"
#include "raymath.h"

constexpr const int DOCK_MARGIN = 30;
constexpr const int DOCK_SEPARATION_MARGIN = 30;
constexpr const int LABEL_TEXT_SIZE = 18;
constexpr const int LABEL_MARGIN = 4;
constexpr const float BOX_SIZE_STEP = 9.0f;
constexpr const int DOCK_FLOOR_OFFSET = 38;
constexpr const float TRUCK_DOCK_GAP = 20.0f;
constexpr const float TRUCK_TRAVEL_DISTANCE = 260.0f;
constexpr const float TRUCK_FADE_DURATION = 0.6f;
constexpr const float TRUCK_MOVE_DURATION = 2.0f;
constexpr const float TRUCK_LEAVE_DURATION = 0.5f;
constexpr const float BOXES_START_TIME = TRUCK_MOVE_DURATION * 0.85f;
constexpr const float BOX_APPEAR_DURATION = 0.5f;
constexpr const float BOX_APPEAR_STAGGER = 0.35f;
constexpr const float BOX_DROP_DISTANCE = 40.0f;
constexpr const float SIMULATION_VERTICAL_OFFSET = 60.0f;
constexpr const float HIGHLIGHT_THICKNESS = 2.0f;
constexpr const float MIN_SOLUTION_PLAYBACK_SPEED = 0.5f;
constexpr const float MAX_SOLUTION_PLAYBACK_SPEED = 5.0f;
constexpr const float BASE_MOVEMENT_DURATION = 1.0f;

static float easeOutCubic(float t) {
	return 1.0f - (1.0f - t) * (1.0f - t) * (1.0f - t);
}

static SimulationDock& lookupDockByBoxIndex(GameSimulation& simulation, int boxIndex) {
	for (SimulationDock* dock : simulation.getDocks()) {
		if (!dock->boxes.empty() && dock->boxes.back().value == boxIndex) {
			return *dock;
		}
	}
}

static SimulationDock& lookupDock(GameSimulation& simulation, FrameStewartSolver::Destination destination) {
	switch (destination.type) {
	case FrameStewartSolver::DockType::Origin:
		return simulation.originDock;
	case FrameStewartSolver::DockType::Destination:
		return simulation.destinationDock;
	case FrameStewartSolver::DockType::Auxiliary:
		return simulation.auxiliarDockList[destination.auxiliarIndex];
	}	
}

SimulationBox::SimulationBox(GameSimulation& simulation, int value) :
	simulation(simulation), value(value), alpha(0.0f), offsetY(-BOX_DROP_DISTANCE) {
}

float SimulationBox::getSize() {
	return this->simulation.getBoxInitialSize() + this->value * BOX_SIZE_STEP;
}

void SimulationBox::setAppearProgress(float progress) {
	this->alpha = progress;
	this->offsetY = -BOX_DROP_DISTANCE * (1.0f - easeOutCubic(progress));
}

void SimulationBox::draw(float centerX, float bottom, bool movable) {
	Texture2D texture = this->simulation.getBoxTexture();
	float size = this->getSize();
	Rectangle source = { 0, 0, (float)texture.width, (float)texture.height };
	Rectangle destination = { centerX - size / 2.0f, bottom - size + this->offsetY, size, size };
	if (
		this->simulation.playingSolution &&
		this->simulation.currentSolutionMovement.has_value()
	) {
		SolutionMovement movement = (*this->simulation.currentSolutionMovement);
		if (movement.boxIndex == this->value && !movement.completed) {
			Rectangle desiredDestination = movement.destinationDock.getPlacementRect(size);
			Vector2 currentDestination = Vector2Lerp(
				{ destination.x, destination.y },
				{ desiredDestination.x, desiredDestination.y },
				easeOutCubic(movement.progress)
			);
			destination = { currentDestination.x, currentDestination.y, size, size };
		}
	}
	DrawTexturePro(texture, source, destination, { 0, 0 }, 0, Fade(WHITE, this->alpha));

	if (
		!movable ||
		this->simulation.playingBeginAnimation ||
		this->simulation.playingSolution ||
		this->simulation.isDragging() ||
		this->simulation.areAllBoxesInDestination()
	) return;

	Vector2 mousePos = GetMousePosition();
	if (CheckCollisionPointRec(mousePos, destination)) {
		DrawRectangleLinesEx(destination, HIGHLIGHT_THICKNESS, WHITE);
	}
}

void SimulationBox::drawGhost(float centerX, float bottom, float opacity) {
	Texture2D texture = this->simulation.getBoxTexture();
	float size = this->getSize();
	Rectangle source = { 0, 0, (float)texture.width, (float)texture.height };
	Rectangle destination = { centerX - size / 2.0f, bottom - size, size, size };
	DrawTexturePro(texture, source, destination, { 0, 0 }, 0, Fade(WHITE, opacity));
}

SimulationDock::SimulationDock(GameSimulation& simulation, FrameStewartSolver::DockType type, int index) :
	simulation(simulation), type(type), index(index) {
}

void SimulationDock::drawBoxes() {
	Texture2D dockTexture = this->simulation.getDockTexture();
	Vector2 position = this->getPosition();
	int x = (int)position.x;
	int y = (int)position.y;

	float centerX = x + dockTexture.width / 2.0f;
	float bottom = y + dockTexture.height - DOCK_FLOOR_OFFSET;

	for (size_t i = 0; i < this->boxes.size(); ++i) {
		SimulationBox& box = this->boxes[i];
		if (!this->simulation.isBeingDragged(box)) {
			box.draw(centerX, bottom, i == this->boxes.size() - 1);
		}
		bottom -= box.getSize();
	}
}

Rectangle SimulationDock::getRect() {
	Texture2D dockTexture = this->simulation.getDockTexture();
	Vector2 position = this->getPosition();
	return { position.x, position.y, (float)dockTexture.width, (float)dockTexture.height };
}

Vector2 SimulationDock::getStackAnchor(size_t count) {
	Texture2D dockTexture = this->simulation.getDockTexture();
	Vector2 position = this->getPosition();

	float centerX = position.x + dockTexture.width / 2.0f;
	float bottom = position.y + dockTexture.height - DOCK_FLOOR_OFFSET;
	for (size_t i = 0; i < count && i < this->boxes.size(); ++i) {
		bottom -= this->boxes[i].getSize();
	}
	return { centerX, bottom };
}

Rectangle SimulationDock::getPlacementRect(float boxSize) {
	Vector2 anchor = this->getStackAnchor(this->boxes.size());
	return { anchor.x - boxSize / 2.0f, anchor.y - boxSize, boxSize, boxSize };
}

Rectangle SimulationDock::getTopBoxRect() {
	Vector2 anchor = this->getStackAnchor(this->boxes.size() - 1);
	float size = this->boxes.back().getSize();
	return { anchor.x - size / 2.0f, anchor.y - size, size, size };
}

bool SimulationDock::canAccept(SimulationBox& box) {
	return this->boxes.empty() || box.value < this->boxes.back().value;
}

Vector2 SimulationDock::getPosition() {
	Texture2D dockTexture = this->simulation.getDockTexture();

	int renderWidth = GetRenderWidth();
	int renderHeight = GetRenderHeight();
	int bottomRowY = renderHeight - GameLayout::STATUS_BAR_HEIGHT - dockTexture.height - DOCK_MARGIN;

	switch (this->type) {
	case FrameStewartSolver::DockType::Origin:
		return { (float)DOCK_MARGIN, (float)bottomRowY };
	case FrameStewartSolver::DockType::Destination:
		return { (float)(renderWidth - dockTexture.width - DOCK_MARGIN), (float)bottomRowY };
	default:
		int count = this->simulation.auxiliarDocks;
		int rowWidth = count * dockTexture.width + (count - 1) * DOCK_SEPARATION_MARGIN;
		int x = (renderWidth - rowWidth) / 2 + this->index * (dockTexture.width + DOCK_SEPARATION_MARGIN);
		int y = bottomRowY - dockTexture.height - DOCK_SEPARATION_MARGIN - SIMULATION_VERTICAL_OFFSET;
		return { (float)x, (float)y };
	}
}

void SimulationDock::draw() {
	Texture2D dockTexture = this->simulation.getDockTexture();

	Vector2 position = this->getPosition();
	int x = (int)position.x;
	int y = (int)position.y;
	DrawTexture(dockTexture, x, y, WHITE);

	Font labelFont = this->simulation.getLabelFont();
	const char* label;
	switch (this->type) {
	case FrameStewartSolver::DockType::Origin:
		label = "Muelle origen";
		break;
	case FrameStewartSolver::DockType::Destination:
		label = "Muelle destino";
		break;
	default:
		label = TextFormat("Muelle auxiliar %d", this->index + 1);
		break;
	}
	Vector2 labelSize = MeasureTextEx(labelFont, label, LABEL_TEXT_SIZE, 0);
	DrawTextEx(labelFont, label, { x + (dockTexture.width - labelSize.x) / 2, (float)(y + dockTexture.height + LABEL_MARGIN) }, LABEL_TEXT_SIZE, 0, WHITE);
}

SolutionMovement::SolutionMovement(
	GameSimulation& simulation,
	FrameStewartSolver::Movement movement
) :
	simulation(simulation), boxIndex(movement.boxIndex),
	sourceDock(lookupDockByBoxIndex(this->simulation, movement.boxIndex)),
	destinationDock(lookupDock(this->simulation, movement.destination)),
	progress(0.0f), completed(false) {
}

void SolutionMovement::update(float dt) {
	if (this->completed) return;
	this->progress += dt / this->simulation.getSolutionMovementDuration();
	if (this->progress >= 1.0f) {
		this->progress = 1.0f;
		this->completed = true;

		this->destinationDock.boxes.push_back(this->sourceDock.boxes.back());
		this->sourceDock.boxes.pop_back();
	}
}

GameSimulation::GameSimulation() :
	solver(), originDock(*this, FrameStewartSolver::DockType::Origin), destinationDock(*this, FrameStewartSolver::DockType::Destination),
	animationTime(0), truckAlpha(0), truckOffset(0), truckVisible(false), truckLeaving(false), truckLeaveTime(0),
	draggedBox(nullptr), dragSource(nullptr), dragGrabOffset({ 0, 0 }),
	boxes(0), auxiliarDocks(0), simulatedAtLeastOnce(false), playingBeginAnimation(false), playingSolution(false), solutionPlaybackSpeed(1.0f) {}

bool GameSimulation::isDragging() {
	return this->draggedBox != nullptr;
}

bool GameSimulation::isBeingDragged(SimulationBox& box) {
	return &box == this->draggedBox;
}

bool GameSimulation::areAllBoxesInDestination() {
	return this->destinationDock.boxes.size() == this->boxes;
}

std::vector<SimulationDock*> GameSimulation::getDocks() {
	std::vector<SimulationDock*> docks = { &this->originDock, &this->destinationDock };
	for (SimulationDock& dock : this->auxiliarDockList) {
		docks.push_back(&dock);
	}
	return docks;
}

Texture2D GameSimulation::getDockTexture() {
	if (!this->dockTexture.has_value()) {
		Image image = LoadImageFromMemory(".png", GameSprites::DOCK->data, GameSprites::DOCK->size);
		this->dockTexture = LoadTextureFromImage(image);
		UnloadImage(image);
	}
	return *this->dockTexture;
}

Texture2D GameSimulation::getBoxTexture() {
	if (!this->boxTexture.has_value()) {
		Image image = LoadImageFromMemory(".png", GameSprites::BOX->data, GameSprites::BOX->size);
		this->boxTexture = LoadTextureFromImage(image);
		UnloadImage(image);
	}
	return *this->boxTexture;
}

Texture2D GameSimulation::getTruckTexture() {
	if (!this->truckTexture.has_value()) {
		Image image = LoadImageFromMemory(".png", GameSprites::TRUCK->data, GameSprites::TRUCK->size);
		this->truckTexture = LoadTextureFromImage(image);
		UnloadImage(image);
	}
	return *this->truckTexture;
}

Font GameSimulation::getLabelFont() {
	if (!this->labelFont.has_value()) {
		this->labelFont = LoadFontFromMemory(".ttf", GameFonts::UBUNTU_LIGHT->data, GameFonts::UBUNTU_LIGHT->size, LABEL_TEXT_SIZE, nullptr, 250);
	}
	return *this->labelFont;
}

float GameSimulation::getBoxInitialSize() {
	switch (this->boxes) {
	case 2:
		return 60.0f;
	case 3:
		return 38.0f;
	case 4:
		return 22.0f;
	default:
		return 11.0f;
	}
}

void GameSimulation::begin(int boxes, int auxiliarDocks) {
	if (!this->simulatedAtLeastOnce) {
		this->simulatedAtLeastOnce = true;
	}

	this->boxes = boxes;
	this->auxiliarDocks = auxiliarDocks;

	this->draggedBox = nullptr;
	this->dragSource = nullptr;

	this->originDock.boxes.clear();
	this->destinationDock.boxes.clear();

	for (int i = boxes - 1; i >= 0; --i) {
		this->originDock.boxes.emplace_back(*this, i);
	}

	this->auxiliarDockList.clear();
	this->auxiliarDockList.reserve(auxiliarDocks);
	for (int i = 0; i < auxiliarDocks; ++i) {
		this->auxiliarDockList.emplace_back(*this, FrameStewartSolver::DockType::Auxiliary, i);
	}

	this->playingBeginAnimation = true;
	this->animationTime = 0.0f;
	this->truckLeaving = false;
	this->truckLeaveTime = 0.0f;
	this->truckAlpha = 0.0f;
	this->truckOffset = TRUCK_TRAVEL_DISTANCE;
	this->truckVisible = true;
	this->playingSolution = false;
	this->solution.clear();
	this->currentSolutionMovement.reset();
}

void GameSimulation::beginAndSolve(int boxes, int auxiliarDocks) {
	this->begin(boxes, auxiliarDocks);
	this->solution = this->solver.solve(boxes, auxiliarDocks + 2);
	this->playingSolution = true;
}

void GameSimulation::drawTruck() {
	if (!this->truckVisible) return;

	Texture2D truckTexture = this->getTruckTexture();
	Texture2D dockTexture = this->getDockTexture();
	Vector2 dockPosition = this->originDock.getPosition();

	float x = dockPosition.x + dockTexture.width + TRUCK_DOCK_GAP + this->truckOffset;
	float y = dockPosition.y + dockTexture.height - truckTexture.height;
	DrawTexture(truckTexture, x, y, Fade(WHITE, this->truckAlpha));
}

void GameSimulation::drawDrag() {
	if (!this->draggedBox) return;

	Vector2 mousePos = GetMousePosition();

	for (SimulationDock* dock : this->getDocks()) {
		if (dock == this->dragSource || !dock->canAccept(*this->draggedBox)) continue;

		Vector2 anchor = dock->getStackAnchor(dock->boxes.size());
		float opacity = CheckCollisionPointRec(mousePos, dock->getRect()) ? 0.6f : 0.3f;
		this->draggedBox->drawGhost(anchor.x, anchor.y, opacity);
	}

	float size = this->draggedBox->getSize();
	float simulationBottom = (float)(GetRenderHeight() - GameLayout::STATUS_BAR_HEIGHT);
	float centerX = std::clamp(mousePos.x - this->dragGrabOffset.x, size / 2.0f, GetRenderWidth() - size / 2.0f);
	float bottom = std::clamp(mousePos.y - this->dragGrabOffset.y + size / 2.0f, GameLayout::SIMULATION_TOP + size, simulationBottom);
	this->draggedBox->draw(centerX, bottom, false);
}

void GameSimulation::draw() {
	if (!this->simulatedAtLeastOnce) return;

	this->originDock.draw();
	this->destinationDock.draw();
	for (SimulationDock& dock : this->auxiliarDockList) {
		dock.draw();
	}

	this->originDock.drawBoxes();
	this->destinationDock.drawBoxes();
	for (SimulationDock& dock : this->auxiliarDockList) {
		dock.drawBoxes();
	}

	this->drawTruck();
	this->drawDrag();
}

void GameSimulation::updateDrag() {
	Vector2 mousePos = GetMousePosition();

	if (this->playingBeginAnimation || this->playingSolution || this->areAllBoxesInDestination()) {
		this->draggedBox = nullptr;
		this->dragSource = nullptr;
		return;
	}

	if (!this->draggedBox) {
		if (!IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) return;

		for (SimulationDock* dock : this->getDocks()) {
			if (dock->boxes.empty()) continue;

			Rectangle rect = dock->getTopBoxRect();
			if (CheckCollisionPointRec(mousePos, rect)) {
				this->draggedBox = &dock->boxes.back();
				this->dragSource = dock;
				this->dragGrabOffset = { mousePos.x - (rect.x + rect.width / 2.0f), mousePos.y - (rect.y + rect.height / 2.0f) };
				return;
			}
		}
		return;
	}

	if (!IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) return;

	for (SimulationDock* dock : this->getDocks()) {
		if (dock == this->dragSource) continue;
		if (CheckCollisionPointRec(mousePos, dock->getRect()) && dock->canAccept(*this->draggedBox)) {
			dock->boxes.push_back(*this->draggedBox);
			this->dragSource->boxes.pop_back();
			break;
		}
	}

	this->draggedBox = nullptr;
	this->dragSource = nullptr;
}

void GameSimulation::update(float dt) {
	this->updateDrag();

	if (this->playingBeginAnimation) {
		if (this->truckLeaving) {
			this->truckLeaveTime += dt;
			float t = std::min(this->truckLeaveTime / TRUCK_LEAVE_DURATION, 1.0f);
			this->truckAlpha = 1.0f - t;
			if (t >= 1.0f) {
				this->truckVisible = false;
				this->truckLeaving = false;
				this->playingBeginAnimation = false;
			}
			return;
		}

		this->animationTime += dt;

		float moveTime = std::min(this->animationTime, TRUCK_MOVE_DURATION);
		this->truckAlpha = std::min(moveTime / TRUCK_FADE_DURATION, 1.0f);
		this->truckOffset = TRUCK_TRAVEL_DISTANCE * (1.0f - easeOutCubic(moveTime / TRUCK_MOVE_DURATION));

		std::vector<SimulationBox>& boxes = this->originDock.boxes;
		for (size_t i = 0; i < boxes.size(); ++i) {
			float localTime = this->animationTime - BOXES_START_TIME - i * BOX_APPEAR_STAGGER;
			float t = std::clamp(localTime / BOX_APPEAR_DURATION, 0.0f, 1.0f);
			boxes[i].setAppearProgress(t);
		}

		float lastBoxIndex = boxes.empty() ? 0.0f : (float)(boxes.size() - 1);
		float endTime = BOXES_START_TIME + lastBoxIndex * BOX_APPEAR_STAGGER + BOX_APPEAR_DURATION;
		if (this->animationTime >= endTime) {
			this->truckLeaving = true;
			this->truckLeaveTime = 0.0f;
		}
	}

	if (this->playingSolution && !this->playingBeginAnimation) {
		this->consumeNextSolutionMovement();
		if (this->currentSolutionMovement.has_value()) {
			(*this->currentSolutionMovement).update(dt);
		}
		else {
			this->playingSolution = false;
		}
	}
}

void GameSimulation::consumeNextSolutionMovement() {
	if (this->currentSolutionMovement.has_value() && !(*this->currentSolutionMovement).completed) {
		return;
	}
	if (this->solution.empty()) {
		this->currentSolutionMovement.reset();
		return;
	}

	FrameStewartSolver::Movement& rawMovement = this->solution.front();
	this->currentSolutionMovement.emplace(*this, rawMovement);

	this->solution.pop_front();
}

void GameSimulation::increaseSolutionSpeed() {
	this->solutionPlaybackSpeed = std::min(MAX_SOLUTION_PLAYBACK_SPEED, this->solutionPlaybackSpeed + 0.5f);
}

void GameSimulation::decreaseSolutionSpeed() {
	this->solutionPlaybackSpeed = std::max(MIN_SOLUTION_PLAYBACK_SPEED, this->solutionPlaybackSpeed - 0.5f);
}

float GameSimulation::getSolutionMovementDuration() {
	return BASE_MOVEMENT_DURATION / this->solutionPlaybackSpeed;
}

void GameSimulation::unload() {
	if (this->dockTexture.has_value()) {
		UnloadTexture(*this->dockTexture);
	}
	if (this->boxTexture.has_value()) {
		UnloadTexture(*this->boxTexture);
	}
	if (this->truckTexture.has_value()) {
		UnloadTexture(*this->truckTexture);
	}
	if (this->labelFont.has_value()) {
		UnloadFont(*this->labelFont);
	}
}

