#include "game_title.hpp"
#include <raylib.h>
#include <stdio.h>
#include "../game_globals.hpp"

GameTitle::GameTitle() {}

Font GameTitle::getMainFont() {
    if (!this->mainFont.has_value()) {
        this->mainFont = LoadFontFromMemory(".ttf", GameFonts::UBUNTU_LIGHT->data, GameFonts::UBUNTU_LIGHT->size, 32, nullptr, 250);
    }
    return *this->mainFont;
}

Font GameTitle::getSecondaryFont() {
    if (!this->secondaryFont.has_value()) {
        this->secondaryFont = LoadFontFromMemory(".ttf", GameFonts::UBUNTU_LIGHT->data, GameFonts::UBUNTU_LIGHT->size, 20, nullptr, 250);
    }
    return *this->secondaryFont;
}

void GameTitle::draw() {
    int renderWidth = GetRenderWidth();

    Font mainFont = this->getMainFont();
    Font secondaryFont = this->getSecondaryFont();

    DrawTextEx(mainFont, "Juego de simulación de descarga.", { 10, 10 }, 32, 0, WHITE);
    DrawTextEx(secondaryFont, "Estudiante: Carlos Barranca.", { 10, 45 }, 20, 0, WHITE);
}

void GameTitle::unload() {
    if (this->mainFont.has_value()) {
        UnloadFont(*this->mainFont);
    }
    if (this->secondaryFont.has_value()) {
        UnloadFont(*this->secondaryFont);
    }
}