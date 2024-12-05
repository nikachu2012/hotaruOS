#include "pixelWriter.hpp"

constexpr uint32_t RGBResv8BitColorGen(const PixelTrueColor &c)
{
    return (c.b << 16) | (c.g << 8) | c.r;
}

constexpr uint32_t RGBResv8BitColorGen(const PixelRGBA &c)
{
    return (c.b << 16) | (c.g << 8) | c.r;
}

constexpr uint32_t BGRResv8BitColorGen(const PixelTrueColor &c)
{
    return (c.r << 16) | (c.g << 8) | c.b;
}

constexpr uint32_t BGRResv8BitColorGen(const PixelRGBA &c)
{
    return (c.r << 16) | (c.g << 8) | c.b;
}

constexpr size_t getAddr(const frameBufferConfig &c, int x, int y)
{
    return c.widthResolution * y + x;
}

void RGBResv8BitPerColorPixelWriter::drawPixel(int x, int y, const PixelTrueColor &c)
{
    config_.frameBuffer[getAddr(config_, x, y)] = RGBResv8BitColorGen(c);
}

void RGBResv8BitPerColorPixelWriter::drawRect(int x, int y, int width, int height, const PixelTrueColor &c)
{
    auto color = RGBResv8BitColorGen(c);
    // 上下
    for (size_t dx = 0; dx < width; dx++)
    {
        config_.frameBuffer[getAddr(config_, x + dx, y)] = color;
        config_.frameBuffer[getAddr(config_, x + dx, y + height)] = color;
    }

    // 左右
    for (int dy = 0; dy < height; dy++)
    {
        config_.frameBuffer[getAddr(config_, x, y + dy)] = color;
        config_.frameBuffer[getAddr(config_, x + width, y + dy)] = color;
    }
}

void RGBResv8BitPerColorPixelWriter::drawRectWithFill(int x, int y, int width, int height, const PixelTrueColor &c)
{
    auto color = RGBResv8BitColorGen(c);

    for (size_t dy = 0; dy < height; dy++)
    {
        auto startptr = config_.frameBuffer + getAddr(config_, x, y + dy);
        for (size_t dx = 0; dx < width; dx++)
        {
            startptr[dx] = color;
        }
    }
}

void RGBResv8BitPerColorPixelWriter::drawImageRGBA(int x, int y, const struct PixelRGBA *i, int width, int height)
{
    for (size_t dy = 0; dy < height; dy++)
    {
        auto startptr = config_.frameBuffer + (config_.pixelPerScanLine * (y + dy) + x);

        for (size_t dx = 0; dx < width; dx++)
        {
            if (i[width * dy + dx].a == 0)
                continue;

            startptr[dx] = RGBResv8BitColorGen(i[width * dy + dx]);
        }
    }
}

void BGRResv8BitPerColorPixelWriter::drawPixel(int x, int y, const PixelTrueColor &c)
{
    config_.frameBuffer[getAddr(config_, x, y)] = BGRResv8BitColorGen(c);
}

void BGRResv8BitPerColorPixelWriter::drawRect(int x, int y, int width, int height, const PixelTrueColor &c)
{
    auto color = RGBResv8BitColorGen(c);

    // 上下
    for (size_t dx = 0; dx < width; dx++)
    {
        config_.frameBuffer[getAddr(config_, x + dx, y)] = color;
        config_.frameBuffer[getAddr(config_, x + dx, y + height)] = color;
    }

    // 左右
    for (int dy = 0; dy < height; dy++)
    {
        config_.frameBuffer[getAddr(config_, x, y + dy)] = color;
        config_.frameBuffer[getAddr(config_, x + width, y + dy)] = color;
    }
}

void BGRResv8BitPerColorPixelWriter::drawRectWithFill(int x, int y, int width, int height, const PixelTrueColor &c)
{
    auto color = BGRResv8BitColorGen(c);

    for (size_t dy = 0; dy < height; dy++)
    {
        auto startptr = config_.frameBuffer + getAddr(config_, x, y + dy);
        for (size_t dx = 0; dx < width; dx++)
        {
            startptr[dx] = color;
        }
    }
}

void BGRResv8BitPerColorPixelWriter::drawImageRGBA(int x, int y, const PixelRGBA *i, int width, int height)
{
    for (size_t dy = 0; dy < height; dy++)
    {
        auto startptr = config_.frameBuffer + (config_.pixelPerScanLine * (y + dy) + x);

        for (size_t dx = 0; dx < width; dx++)
        {
            if (i[width * dy + dx].a == 0)
                continue;

            startptr[dx] = BGRResv8BitColorGen(i[width * dy + dx]);
        }
    }
}
