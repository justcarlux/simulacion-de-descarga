#pragma once

#include <raylib.h>
#include <optional>
#include <deque>
#include <vector>
#include "../../unload_solver.h"

class GameSimulation;

class SimulationBox {
private:
	GameSimulation& simulation;
	float alpha;
	float offsetY;
	void drawSprites(Rectangle bounds, Color tint);
public:
	SimulationBox(GameSimulation& simulation, BoxType type);
	BoxType type;
	float getWidth();
	float getHeight();
	Rectangle getRect(float centerX, float bottom);
	void setAppearProgress(float progress);
	void draw(float centerX, float bottom, bool movable);
	void drawGhost(float centerX, float bottom, float opacity);
};

class SimulationDock {
private:
	GameSimulation& simulation;
public:
	std::vector<SimulationBox> boxes;
	UnloadSolver::DockType type;
	int index;
	SimulationDock(GameSimulation& simulation, UnloadSolver::DockType type, int auxiliarIndex = 0);
	Vector2 getPosition();
	Rectangle getRect();
	Vector2 getStackAnchor(size_t count);
	Rectangle getPlacementRect(SimulationBox& box);
	Rectangle getTopBoxRect();
	bool canAccept(SimulationBox& box);
	void draw();
	void drawBoxes();
};

class SolutionMovement {
private:
	GameSimulation& simulation;
public:
	SimulationDock& sourceDock;
	float progress;
	bool completed;
	SimulationDock& destinationDock;
	SolutionMovement(
		GameSimulation& simulation,
		UnloadSolver::Movement movement
	);
	void update(float dt);
};

class GameSimulation
{
private:
	UnloadSolver solver;
	std::optional<Texture2D> dockTexture;
	std::optional<Texture2D> boxTexture;
	std::optional<Texture2D> truckTexture;
	std::optional<Font> labelFont;
	float animationTime;
	float truckAlpha;
	float truckOffset;
	bool truckVisible;
	bool truckLeaving;
	float truckLeaveTime;
	SimulationBox* draggedBox;
	SimulationDock* dragSource;
	Vector2 dragGrabOffset;
	void updateDrag();
	void drawDrag();
	void drawTruck();
public:
	int bigBoxes;
	int smallGroups;
	int auxiliarDocks;
	bool simulatedAtLeastOnce;
	bool playingBeginAnimation;
	bool playingSolution;
	float solutionPlaybackSpeed;
	std::optional<SolutionMovement> currentSolutionMovement;
	SimulationDock originDock;
	SimulationDock destinationDock;
	std::vector<SimulationDock> auxiliarDockList;
	std::deque<UnloadSolver::Movement> solution;
	GameSimulation();
	std::vector<SimulationDock*> getDocks();
	bool isDragging();
	bool isBeingDragged(SimulationBox& box);
	bool areAllBoxesInDestination();
	Texture2D getDockTexture();
	Texture2D getBoxTexture();
	Texture2D getTruckTexture();
	Font getLabelFont();
	int getTotalBoxes();
	float getBoxScale();
	void begin(int bigBoxes, int smallGroups, int auxiliarDocks);
	void beginAndSolve(int bigBoxes, int smallGroups, int auxiliarDocks);
	void draw();
	void update(float dt);
	void consumeNextSolutionMovement();
	void increaseSolutionSpeed();
	void decreaseSolutionSpeed();
	float getSolutionMovementDuration();
	void unload();
};
