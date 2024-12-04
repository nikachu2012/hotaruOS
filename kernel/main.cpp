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
#include <cstdarg>
#include "serial/serial.hpp"
#include "graphics/frameBufferConfig.hpp"
#include "graphics/pixelWriter.hpp"
#include "font/font.hpp"
#include "console/console.hpp"

constexpr int BUF_SIZ = 1024;

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

uint8_t consoleBuf[sizeof(Console)];
Console *console;

// カーネルからの出力
int printk(const char *c, ...)
{
    va_list ap;
    va_start(ap, c);

    char buf[BUF_SIZ];
    int result = vsnprintf(buf, BUF_SIZ, c, ap);

    console->puts(buf);
    return result;
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

    console = new (consoleBuf) Console(*pixelWriter, {0, 0, 0}, {0xff, 0xff, 0xff});

    pixelWriter->writeRect(0, 0, frameBufferConfig.widthResolution, frameBufferConfig.heightResolution, {0, 0xff, 0});
    pixelWriter->writeRect(100, 100, 200, 100, {0xdc, 0xdc, 0xdc});

    // writeString(*pixelWriter, 5, 3, "hotaruOS  File  Edit  View  Label  Special", {0x0, 0x0, 0x0});

    char buf[256];
    snprintf(buf, 256, "1 + 2 = %d", 3);
    writeString(*pixelWriter, 0, 200, buf, {0, 0, 0});

    for (size_t i = 0; i < 200; i++)
    {
        printk("line: %d test", i);
    }

    Halt();
}
