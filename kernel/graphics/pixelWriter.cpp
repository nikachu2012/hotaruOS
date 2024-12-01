#include "pixelWriter.hpp"

void RGBResv8BitPerColorPixelWriter::write(int x, int y, const PixelTrueColor &c)
{
    auto p = getPixelIndex(x, y);
    p[0] = c.r;
    p[1] = c.g;
    p[2] = c.b;
}

uint8_t *RGBResv8BitPerColorPixelWriter::getPixelIndex(int x, int y)
{
    return config_.frameBuffer + 4 * (config_.pixelPerScanLine * y + x);
}

void BGRResv8BitPerColorPixelWriter::write(int x, int y, const PixelTrueColor &c)
{
    auto p = getPixelIndex(x, y);
    p[0] = c.b;
    p[1] = c.g;
    p[2] = c.r;
}

uint8_t *BGRResv8BitPerColorPixelWriter::getPixelIndex(int x, int y)
{
    return config_.frameBuffer + 4 * (config_.pixelPerScanLine * y + x);
}
