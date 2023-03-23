#pragma once

#include "graphics.hpp"

void WriteAscii(PixelWriter &writer, Vector2D<int> pos, char c,
                const PixelColor &color);
void WriteString(PixelWriter &writer, Vector2D<int>, const char *s,
                 const PixelColor &color);