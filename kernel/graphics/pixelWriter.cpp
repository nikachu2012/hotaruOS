#include "pixelWriter.hpp"

constexpr uint32_t RGBResv8BitColorGen(const PixelTrueColor &c)
{
    return (c.b << 16) | (c.g << 8) | c.r;
}

constexpr uint32_t BGRResv8BitColorGen(const PixelTrueColor &c)
{
    return (c.r << 16) | (c.g << 8) | c.b;
}

constexpr void drawRectBase(frameBufferConfig config, int x, int y, int width, int height, uint32_t color)
{
    uint32_t *startptr = nullptr;

    // 上
    startptr = config.frameBuffer + (config.pixelPerScanLine * y + x);
    for (size_t dx = 0; dx < width; dx++)
    {
        startptr[dx] = color;
    }

    // 左右
    startptr = config.frameBuffer;
    for (int dy = y; dy < y + height; dy++)
    {
        startptr[config.pixelPerScanLine * dy + x] = color;
        startptr[config.pixelPerScanLine * dy + x + width] = color;
    }

    // 下
    startptr = config.frameBuffer + (config.pixelPerScanLine * (y + height) + x);
    for (size_t dx = 0; dx < width + 1; dx++)
    {
        startptr[dx] = color;
    }
}

void RGBResv8BitPerColorPixelWriter::writePixel(int x, int y, const PixelTrueColor &c)
{
    auto p = config_.frameBuffer + (config_.pixelPerScanLine * y + x);

    *p = RGBResv8BitColorGen(c);
}

void RGBResv8BitPerColorPixelWriter::drawRect(int x, int y, int width, int height, const PixelTrueColor &c)
{
    auto color = RGBResv8BitColorGen(c);
    drawRectBase(config_, x, y, width, height, color);
}

void RGBResv8BitPerColorPixelWriter::writeRectWithFill(int x, int y, int width, int height, const PixelTrueColor &c)
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

void BGRResv8BitPerColorPixelWriter::drawRect(int x, int y, int width, int height, const PixelTrueColor &c)
{
    auto color = RGBResv8BitColorGen(c);
    drawRectBase(config_, x, y, width, height, color);
}

void BGRResv8BitPerColorPixelWriter::writeRectWithFill(int x, int y, int width, int height, const PixelTrueColor &c)
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
