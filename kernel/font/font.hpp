#pragma once

#include <stdint.h>
#include "../graphics/pixelWriter.hpp"

#define FONT_WIDTH 8
#define FONT_HEIGHT 16

const uint8_t FONT_A[FONT_HEIGHT] = {
    0b00000000,
    0b00011000,
    0b00011000,
    0b00011000,
    0b00011000,
    0b00100100,
    0b00100100,
    0b00100100,
    0b00100100,
    0b01111110,
    0b01000010,
    0b01000010,
    0b11100111,
    0b00000000,
    0b00000000};

void writeChar(PixelWriter &writer, int x, int y, char c, const PixelTrueColor &color);
