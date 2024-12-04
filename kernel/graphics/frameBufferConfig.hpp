#pragma once

#include <stdint.h>

enum pixelFormat
{
    PixelFormat_undefined,
    /*
        index | 0 | 1 | 2 | 3  | 4 | 5 | 6 | 7  | ... (Re = reserved)
        data  | R | G | B | Re | R | G | B | Re | ...
     */
    PixelFormat_RGBResv8BitPerColor,
    /*
        index | 0 | 1 | 2 | 3  | 4 | 5 | 6 | 7  | ... (Re = reserved)
        data  | B | G | R | Re | B | G | R | Re | ...
    */
    PixelFormat_BGRResv8BitPerColor,
};

struct frameBufferConfig
{
    uint32_t *frameBuffer;
    uint32_t pixelPerScanLine;
    uint32_t widthResolution;  // 画面の横幅
    uint32_t heightResolution; // 画面の縦幅
    enum pixelFormat pixelFormat;
};
