#include "game.hpp"
#include <raylib.h>
#include <iostream>

#define RAYGUI_IMPLEMENTATION
#include <raygui.h>
#include "game_globals.hpp"

Game::Game() {
    GuiLoadStyleFromMemory(GameGuiStyles::DARK->data, GameGuiStyles::DARK->size);

    SetConfigFlags(FLAG_VSYNC_HINT);
    InitWindow(1000, 812, "Juego de simulación de descarga");
}

void Game::draw() {
    #ifdef DEBUG
        SetWindowTitle(TextFormat("Juego de simulación de descarga - FPS: %d", GetFPS()));
    #endif

    ClearBackground(GameColors::BACKGROUND);
    this->gameTitle.draw();
    this->gameControls.draw(&this->gameSimulation);
    this->gameSimulation.draw();
    this->gameStatusBar.draw(this->gameSimulation);
}

void Game::update() {
    float dt = GetFrameTime();
    if (dt > 0.1f) dt = 0.1f;

    this->gameSimulation.update(dt);
}

void Game::run() {
    while (!WindowShouldClose())
    {
        BeginDrawing();
        this->draw();
        this->update();
        EndDrawing();
    }
    this->unload();
    CloseWindow();
}

void Game::unload() {
    this->gameTitle.unload();
    this->gameControls.unload();
    this->gameSimulation.unload();
    this->gameStatusBar.unload();
}