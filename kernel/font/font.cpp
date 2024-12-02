#include "font.hpp"

void writeChar(PixelWriter &writer, int x, int y, char c, const PixelTrueColor &color)
{
    if (c != 'a')
        return;

    for (int dx = 0; dx < FONT_WIDTH; dx++)
    {
        for (int dy = 0; dy < FONT_HEIGHT; dy++)
        {
            if ((FONT_A[dy] << dx) & 0x80u)
            {
                writer.write(x + dx, y + dy, color);
            }
        }
    }
}
