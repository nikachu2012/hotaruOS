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
#include "cursorImage.hpp"
#include "native/io.hpp"
#include "pci/pci.hpp"

constexpr int BUF_SIZ = 1024;

uint8_t pixelWriterBuf[sizeof(RGBResv8BitPerColorPixelWriter)];
PixelWriter *pixelWriter;

uint8_t consoleBuf[sizeof(Console)];
Console *console;

void Halt()
{
    while (true)
        __asm__("hlt");
}

static const PixelTrueColor s_desktopTextColor = {0xff, 0xff, 0xff};
static const PixelTrueColor s_desktopBgColor = {0x51, 0x5c, 0x6b};
static const PixelTrueColor s_desktopTopBarColor = {0x33, 0x33, 0x33};

// リンク時にエラーが出るため追加
void operator delete(void *buf) noexcept
{
}

// カーネルからの出力
int printk(const char *c, ...)
{
    va_list ap;
    va_start(ap, c);

    char buf[BUF_SIZ];
    int result = vsnprintf(buf, BUF_SIZ, c, ap);

    serialPutString(buf, PORT_COM1);
    serialPutString("\r\n", PORT_COM1);
    console->puts(buf);
    return result;
}

/**
 * @brief カーネルのmain関数
 */
extern "C" void kernelMain(const frameBufferConfig &frameBufferConfig)
{
    // init serial
    if (serialInit(PORT_COM1))
    {
        // Halt();
    }

    // init graphics
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
    // init console
    console = new (consoleBuf) Console(*pixelWriter, s_desktopTextColor, s_desktopBgColor, 0, FONT_HEIGHT + 6);

    // draw Background
    pixelWriter->drawRectWithFill(0, 0, frameBufferConfig.widthResolution, frameBufferConfig.heightResolution, s_desktopBgColor);

    // top bar
    pixelWriter->drawRectWithFill(0, 0, frameBufferConfig.widthResolution, FONT_HEIGHT + 5, s_desktopTopBarColor);
    writeString(*pixelWriter, 8, 3, "hotaruOS  Application  Development  Help", s_desktopTextColor);

    printk("hotaruOS build ???? by nikachu2012 \nshhh.... let's not leak our hard work");
    pixelWriter->drawImageRGBA(300, 600, cursorData, cursorDataWidth, cursorDataHeight);

    // PCI scanAllBus
    PciError status = Pci::scanAllBus();
    printk("Pci::scanAllBus : %d", status);

    for (size_t i = 0; i < Pci::m_deviceCount; i++)
    {
        auto d = Pci::m_devices[i];
        printk("%d, %d, %d: Device ID: 0x%04x, Vendor: 0x%04x, Class %02x%02x%02x, Header 0x%02x", d.bus, d.device, d.function, d.deviceID, d.vendorID, d.classCode.baseClass, d.classCode.subClass, d.classCode.interface, d.headerType);
    }

    // search xHC
    Pci::Device *xhc = nullptr;

    for (size_t i = 0; i < Pci::m_deviceCount; i++)
    {
        if (Pci::m_devices[i].classCode.match({0x0cu, 0x03u, 0x30u}))
        {
            // is device xHCI
            xhc = &Pci::m_devices[i];

            if (Pci::m_devices[i].vendorID == 0x8086)
            {
                // is vendor Intel
                break;
            }
        }
    }

    if (xhc)
    {
        printk("xHC: %d, %d, %d", xhc->bus, xhc->device, xhc->function);
    }
    else
        printk("xHC is not found");

    Halt();
}
