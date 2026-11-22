
#include "integer_input_box.hpp"
#include <raylib.h>
#include <charconv>
#include <cstring>
#include "../game_globals.hpp"

constexpr const Color BORDER_COLOR = { 255, 255, 255, 255 };
constexpr const Color BORDER_COLOR_ERROR = RED;
constexpr const Color FILL_COLOR = { 70, 70, 70, 255 };
constexpr const Color FILL_COLOR_SELECTED = { 100, 100, 100, 255 };
constexpr const int CURSOR_WIDTH = 2;
constexpr const int CURSOR_VERTICAL_MARGIN = 4;
constexpr const int CURSOR_HORIZONTAL_MARGIN = 3;

IntegerInputBox::IntegerInputBox(Rectangle bounds, int textSize, int value) :
    bounds(bounds), textSize(textSize), value(value), currentValueValid(true), textBuffer(""), selected(false) {
    this->setupInitialValue();
}

IntegerInputBox::IntegerInputBox(Rectangle bounds, int textSize, int value, int min, int max) :
    bounds(bounds), textSize(textSize), value(value), currentValueValid(true), textBuffer(""), selected(false), range(std::make_pair(min, max)) {
    this->setupInitialValue();
}

void IntegerInputBox::setupInitialValue() {
    std::to_chars_result result = std::to_chars(this->textBuffer, this->textBuffer + sizeof(this->textBuffer), this->value);
    if (result.ec == std::errc()) {
        std::ptrdiff_t length = result.ptr - this->textBuffer;
        this->currentIndex = length;
    }
}

Font IntegerInputBox::getFont() {
    if (!this->font.has_value()) {
        this->font = LoadFontFromMemory(".ttf", GameFonts::UBUNTU_LIGHT->data, GameFonts::UBUNTU_LIGHT->size, this->textSize, 0, 250);
    }
    return *this->font;
}

bool IntegerInputBox::textBufferIsEmpty() {
    return this->currentIndex <= 0;
}

void IntegerInputBox::draw() {
    // Basado ligeramente en: https://www.raylib.com/examples/text/loader.html?name=text_input_box
    Font font = this->getFont();

    if (this->selected) {
        int key = GetCharPressed();
        while (key > 0) {
            char typedKey = (char)key;
            if (
                (std::isdigit(static_cast<unsigned char>(typedKey)) || (this->textBufferIsEmpty() && key == 45)) &&
                (this->currentIndex < MAX_INPUT_CHARACTERS)
                ) {
                this->textBuffer[this->currentIndex] = (char)key;
                this->textBuffer[this->currentIndex + 1] = '\0';
                this->currentIndex++;
            }

            key = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE)) {
            this->currentIndex--;
            if (this->currentIndex < 0) {
                this->currentIndex = 0;
            }
            this->textBuffer[this->currentIndex] = '\0';
        }
    }

    bool hovering = CheckCollisionPointRec(GetMousePosition(), this->bounds);
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        this->selected = hovering;
    }

    this->currentValueValid = true;
    try {
        int value = std::stoi(this->textBuffer);
        if (this->range.has_value()) {
            std::pair<int, int> range = *this->range;
            if (value < range.first) {
                this->currentValueValid = false;
                DrawTextEx(font, TextFormat("El valor es muy pequeño. No puede ser menor a %d", range.first), { this->bounds.x + this->bounds.width + 10, this->bounds.y + 4 }, this->textSize, 0, RED);
            }
            else if (value > range.second) {
                this->currentValueValid = false;
                DrawTextEx(font, TextFormat("El valor es muy grande. No puede ser mayor a %d", range.second), { this->bounds.x + this->bounds.width + 10, this->bounds.y + 4 }, this->textSize, 0, RED);
            }
        }
        if (this->currentValueValid) {
            this->value = value;
        }
    }
    catch (...) {
        this->currentValueValid = false;
        DrawTextEx(font, "Valor inválido.", { this->bounds.x + this->bounds.width + 10, this->bounds.y + 4 }, this->textSize, 0, RED);
    }

    DrawRectangle(this->bounds.x, this->bounds.y, this->bounds.width, this->bounds.height, this->selected ? FILL_COLOR_SELECTED : FILL_COLOR);
    DrawRectangleLines(this->bounds.x, this->bounds.y, this->bounds.width, this->bounds.height, this->currentValueValid ? BORDER_COLOR : BORDER_COLOR_ERROR);

    DrawTextEx(font, this->textBuffer, { this->bounds.x + 4, this->bounds.y + 4 }, this->textSize, 0, WHITE);

    if (this->selected) {
        Vector2 textSize = MeasureTextEx(font, this->textBuffer, this->textSize, 0);
        DrawRectangle(this->bounds.x + textSize.x + CURSOR_WIDTH + CURSOR_HORIZONTAL_MARGIN, this->bounds.y + CURSOR_VERTICAL_MARGIN / 2, CURSOR_WIDTH, this->bounds.height - CURSOR_VERTICAL_MARGIN, WHITE);
    }
}

void IntegerInputBox::unload() {
    if (this->font.has_value()) {
        UnloadFont(*this->font);
    }
}