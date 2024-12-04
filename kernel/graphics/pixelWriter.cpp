#include "pixelWriter.hpp"

constexpr uint32_t RGBResv8BitColorGen(const PixelTrueColor &c)
{
    return (c.b << 16) | (c.g << 8) | c.r;
}

constexpr uint32_t BGRResv8BitColorGen(const PixelTrueColor &c)
{
    return (c.r << 16) | (c.g << 8) | c.b;
}

void RGBResv8BitPerColorPixelWriter::writePixel(int x, int y, const PixelTrueColor &c)
{
    auto p = config_.frameBuffer + (config_.pixelPerScanLine * y + x);

    *p = RGBResv8BitColorGen(c);
}

void RGBResv8BitPerColorPixelWriter::writeRect(int x, int y, int width, int height, const PixelTrueColor &c)
{
    auto color = RGBResv8BitColorGen(c);

    for (size_t dy = 0; dy < height; dy++)
    {
        auto startptr = config_.frameBuffer + (config_.pixelPerScanLine * dy + x);
        for (size_t dx = 0; dx < width; dx++)
        {
            startptr[dx] = color;
        }
    }
}

void BGRResv8BitPerColorPixelWriter::writePixel(int x, int y, const PixelTrueColor &c)
{
    auto p = config_.frameBuffer + (config_.pixelPerScanLine * y + x);

    *p = BGRResv8BitColorGen(c);
}

void BGRResv8BitPerColorPixelWriter::writeRect(int x, int y, int width, int height, const PixelTrueColor &c)
{
    auto color = BGRResv8BitColorGen(c);

    for (size_t dy = y; dy < y + height; dy++)
    {
        auto startptr = config_.frameBuffer + (config_.pixelPerScanLine * dy + x);
        for (size_t dx = 0; dx < width; dx++)
        {
            startptr[dx] = color;
        }
    }
}
