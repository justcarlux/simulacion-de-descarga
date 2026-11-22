#pragma once

#include <raylib.h>
#include <optional>
#include <deque>
#include "../../frame_stewart_solver.h"

class GameSimulation;

class SimulationBox {
private:
	GameSimulation& simulation;
	float alpha;
	float offsetY;
public:
	SimulationBox(GameSimulation& simulation, int value);
	int value;
	float getSize();
	void setAppearProgress(float progress);
	void draw(float centerX, float bottom, bool movable);
	void drawGhost(float centerX, float bottom, float opacity);
};

class SimulationDock {
private:
	GameSimulation& simulation;
public:
	std::vector<SimulationBox> boxes;
	FrameStewartSolver::DockType type;
	int index;
	SimulationDock(GameSimulation& simulation, FrameStewartSolver::DockType type, int auxiliarIndex = 0);
	Vector2 getPosition();
	Rectangle getRect();
	Vector2 getStackAnchor(size_t count);
	Rectangle getPlacementRect(float boxSize);
	Rectangle getTopBoxRect();
	bool canAccept(SimulationBox& box);
	void draw();
	void drawBoxes();
};

class SolutionMovement {
private:
	GameSimulation& simulation;
	SimulationDock& sourceDock;
public:
	int boxIndex;
	float progress;
	bool completed;
	SimulationDock& destinationDock;
	SolutionMovement(
		GameSimulation& simulation,
		FrameStewartSolver::Movement movement
	);
	void update(float dt);
};

class GameSimulation
{
private:
	FrameStewartSolver solver;
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
	int boxes;
	int auxiliarDocks;
	bool simulatedAtLeastOnce;
	bool playingBeginAnimation;
	bool playingSolution;
	float solutionPlaybackSpeed;
	std::optional<SolutionMovement> currentSolutionMovement;
	SimulationDock originDock;
	SimulationDock destinationDock;
	std::vector<SimulationDock> auxiliarDockList;
	std::deque<FrameStewartSolver::Movement> solution;
	GameSimulation();
	std::vector<SimulationDock*> getDocks();
	bool isDragging();
	bool isBeingDragged(SimulationBox& box);
	bool areAllBoxesInDestination();
	Texture2D getDockTexture();
	Texture2D getBoxTexture();
	Texture2D getTruckTexture();
	Font getLabelFont();
	float getBoxInitialSize();
	void begin(int boxes, int auxiliarDocks);
	void beginAndSolve(int boxes, int auxiliarDocks);
	void draw();
	void update(float dt);
	void consumeNextSolutionMovement();
	void increaseSolutionSpeed();
	void decreaseSolutionSpeed();
	float getSolutionMovementDuration();
	void unload();
};
