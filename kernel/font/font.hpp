#pragma once

#include <cstdint>
#include <cstddef>
#include "../graphics/pixelWriter.hpp"

#define FONT_WIDTH 7
#define FONT_HEIGHT 13

const uint8_t *getFont(char c);

void writeChar(PixelWriter &writer, int x, int y, char c, const PixelTrueColor &color);
void writeString(PixelWriter &writer, int x, int y, const char *s, const PixelTrueColor &color);
