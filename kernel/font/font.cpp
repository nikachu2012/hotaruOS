#include "font.hpp"

extern const uint8_t _binary_ProggyClean_bin_start;
extern const uint8_t _binary_ProggyClean_bin_end;
extern const size_t _binary_ProggyClean_bin_size;

const uint8_t *getFont(char c)
{
    uint64_t index = FONT_HEIGHT * c;

    return &_binary_ProggyClean_bin_start + index;
}

void writeChar(PixelWriter &writer, int x, int y, char c, const PixelTrueColor &color)
{
    const uint8_t *font_bitmap = getFont(c);
    for (int dy = 0; dy < FONT_HEIGHT; dy++)
    {
        for (int dx = 0; dx < FONT_WIDTH; dx++)
        {
            if ((font_bitmap[dy] << dx) & 0x40u)
            {
                writer.drawPixel(x + dx, y + dy, color);
            }
        }
    }
}

void writeString(PixelWriter &writer, int x, int y, const char *s, const PixelTrueColor &color)
{
    for (size_t i = 0; s[i] != 0; i++)
    {
        writeChar(writer, x + FONT_WIDTH * i, y, s[i], color);
    }
}
