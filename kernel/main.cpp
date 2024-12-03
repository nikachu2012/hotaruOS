/**
 * @file main.cpp
 * @author nikachu2012
 * @brief カーネルのmain関数が入っているファイルです
 * @version 0.1
 * @date 2024-12-01
 *
 */
#include <cstdint>
#include <cstdio>
#include "serial/serial.hpp"
#include "graphics/frameBufferConfig.hpp"
#include "graphics/pixelWriter.hpp"
#include "font/font.hpp"

void Halt()
{
    while (true)
        __asm__("hlt");
}

// クラスをbufで指定された場所に置く (配置new)
// newの前に自動でコンストラクタの呼び出しが挿入される
void *operator new(size_t size, void *buf)
{
    return buf;
}

// リンク時にエラーが出るため追加
void operator delete(void *buf) noexcept
{
}

uint8_t pixelWriterBuf[sizeof(RGBResv8BitPerColorPixelWriter)];
PixelWriter *pixelWriter;

/**
 * @brief カーネルのmain関数
 */
extern "C" void kernelMain(const frameBufferConfig &frameBufferConfig)
{
    if (serialInit(PORT_COM1))
    {
        Halt();
    }

    const char *str = "\r\nThis is kernel...\r\nhotaruOS Kernel by nikachu2012\r\n";
    serialPutString(str, PORT_COM1);

    switch (frameBufferConfig.pixelFormat)
    {
    case PixelFormat_BGRResv8BitPerColor:
        pixelWriter = new (pixelWriterBuf) BGRResv8BitPerColorPixelWriter(frameBufferConfig);
        break;
    case PixelFormat_RGBResv8BitPerColor:
        pixelWriter = new (pixelWriterBuf) RGBResv8BitPerColorPixelWriter(frameBufferConfig);
    default:
        break;
    }

    for (int x = 0; x < frameBufferConfig.widthResolution; x++)
    {
        for (int y = 0; y < frameBufferConfig.heightResolution; y++)
        {
            pixelWriter->write(x, y, {0xff, 0xff, 0xff});
        }
    }

    for (int x = 100; x < 100 + 200; x++)
    {
        for (int y = 100; y < 100 + 100; y++)
        {
            pixelWriter->write(x, y, {0xdc, 0xdc, 0xdc});
        }
    }

    writeString(*pixelWriter, 5, 3, "hotaruOS", {0x0, 0x0, 0x0});

    char buf[256];
    snprintf(buf, 256, "1 + 2 = %d", 3);
    writeString(*pixelWriter, 0, 200, buf, {0, 0, 0});

    Halt();
}
