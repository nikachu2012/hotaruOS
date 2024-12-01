/**
 * @file main.cpp
 * @author nikachu2012
 * @brief カーネルのmain関数が入っているファイルです
 * @version 0.1
 * @date 2024-12-01
 *
 */
#include <cstdint>
#include "serial/serial.hpp"
#include "graphics/frameBufferConfig.hpp"

void Halt()
{
    while (true)
        __asm__("hlt");
}

/**
 * @brief 24ビットカラーの色を扱う
 */
struct pixelTrueColor
{
    uint8_t r, g, b;
};

/**
 * @brief 指定の1pxを描画
 *
 * @param config フレームバッファ
 * @param x x座標
 * @param y y座標
 * @param c 色
 * @return int
 * @retval 0 成功
 * @retval 1 失敗
 */
int writePixel(const frameBufferConfig &config, int x, int y, const pixelTrueColor &c)
{
    const int index = config.pixelPerScanLine * y + x;

    if (config.pixelFormat == PixelFormat_BGRResv8BitPerColor)
    {
        uint8_t *p = &config.frameBuffer[4 * index];
        p[0] = c.b;
        p[1] = c.g;
        p[2] = c.r;
    }
    else if (config.pixelFormat == PixelFormat_RGBResv8BitPerColor)
    {
        uint8_t *p = &config.frameBuffer[4 * index];
        p[0] = c.r;
        p[1] = c.g;
        p[2] = c.b;
    }
    else
    {
        return 1;
    }

    return 0;
}

/**
 * @brief カーネルのmain関数
 */
extern "C" void kernelMain(const frameBufferConfig &frameBufferConfig)
{
    if (serialInit(PORT_COM1))
    {
        Halt();
    }

    const char *str = "\nThis is kernel...\nhotaruOS Kernel by nikachu2012\n";
    serialPutString(str, PORT_COM1);

    for (int x = 0; x < frameBufferConfig.widthResolution; x++)
    {
        for (int y = 0; y < frameBufferConfig.heightResolution; y++)
        {
            writePixel(frameBufferConfig, x, y, {0x00, 0xff, 0x00});
        }
    }

    Halt();
}
