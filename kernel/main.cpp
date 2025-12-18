/**
 * @file main.cpp
 * @author nikachu2012
 * @brief カーネルのmain関数が入っているファイルです
 * @version 0.1
 * @date 2024-12-01
 *
 */
#include "console/console.hpp"
#include "font/font.hpp"
#include "graphics/frameBufferConfig.hpp"
#include "graphics/pixelWriter.hpp"
#include "interrupt/interrupt.hpp"
#include "interrupt/ioapic.hpp"
#include "interrupt/pic.hpp"
#include "mouse/cursor.hpp"
#include "mouse/mouse.hpp"
#include "pci/pci.hpp"
#include "serial/serial.hpp"
#include "utils/queue.hpp"
#include <array>
#include <cstdarg>
#include <cstdint>
#include <cstdio>

constexpr int BUF_SIZ = 1024;

static const PixelTrueColor s_desktopTextColor = {0xff, 0xff, 0xff};
static const PixelTrueColor s_desktopBgColor = {0x51, 0x5c, 0x6b};
static const PixelTrueColor s_desktopTopBarColor = {0x33, 0x33, 0x33};

uint8_t pixelWriterBuf[sizeof(RGBResv8BitPerColorPixelWriter)];
PixelWriter *pixelWriter;

uint8_t consoleBuf[sizeof(Console)];
Console *console;

uint8_t cursorBuf[sizeof(MouseCursor)];
MouseCursor *cursor;

struct Message
{
    enum Type
    {
        InterruptMouse
    } type;
};

uint8_t mainQueueBuf[sizeof(ArrayQueue<Message>)];
ArrayQueue<Message> *mainQueue;

Message mainQueueData[100];
// std::array<Message, 100> mainQueueData;

// リンク時にエラーが出るため追加
void operator delete(void *buf) noexcept
{
}

void operator delete(void *ptr, std::size_t size) noexcept
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

void Halt()
{
    printk("Halting...");
    while (true)
        __asm__("hlt");
}

__attribute__((interrupt)) void InterruptHandlerPS2Mouse(InterruptFrame *frame)
{
    mainQueue->push({Message::InterruptMouse});

    Interrupt_PIC::endOfInterrupt(12);
    // Interrupt_IOAPIC::endOfInterrupt();
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
        break;
    default:
        break;
    }
    // init console
    console = new (consoleBuf) Console(*pixelWriter, s_desktopTextColor, s_desktopBgColor, 0, FONT_HEIGHT + 6);

    // init Queue
    mainQueue = new (mainQueue) ArrayQueue<Message>(mainQueueData, 100);

    // draw Background
    pixelWriter->drawRectWithFill(0, 0, frameBufferConfig.widthResolution, frameBufferConfig.heightResolution,
                                  s_desktopBgColor);

    // top bar
    pixelWriter->drawRectWithFill(0, 0, frameBufferConfig.widthResolution, FONT_HEIGHT + 5, s_desktopTopBarColor);
    writeString(*pixelWriter, 8, 3, "hotaruOS  Application  Development  Help", s_desktopTextColor);

    // init mouse+cursor
    cursor = new (cursorBuf) MouseCursor(pixelWriter, s_desktopBgColor, 10, 10);
    MousePS2::reset();
    MousePS2::setSampleRate();
    printk("Mouse is resetted.");

    printk("hotaruOS build ???? by nikachu2012 \nshhh.... let's not leak our hard work");
    // pixelWriter->drawImageRGBA(300, 600, cursorData, cursorDataWidth, cursorDataHeight);

    // PCI scanAllBus
    PciError status = Pci::scanAllBus();
    printk("Pci::scanAllBus : %d", status);

    for (size_t i = 0; i < Pci::m_deviceCount; i++)
    {
        auto d = Pci::m_devices[i];
        printk("%d, %d, %d: Device ID: 0x%04x, Vendor: 0x%04x, Class %02x%02x%02x, Header 0x%02x", d.bus, d.device,
               d.function, d.deviceID, d.vendorID, d.classCode.baseClass, d.classCode.subClass, d.classCode.interface,
               d.headerType);
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

    // 割り込みベクタの登録
    const uint16_t codeSegment = GetCodeSegment();
    IDTR idtr = {sizeof(idt) - 1, reinterpret_cast<uint64_t>(&idt[0])};
    LoadIDT(&idtr);

    // PICを用いたマウスの割り込み有効
    SetIDTEntry(idt[12 + 0x20], CreateIDTAttr(DescriptorType::kInterruptGate, 0),
                reinterpret_cast<uint64_t>(InterruptHandlerPS2Mouse), codeSegment);
    Interrupt_PIC::remap();
    Interrupt_PIC::enable(2);
    Interrupt_PIC::enable(12);

    // APICを用いたマウスの割り込み有効
    // こちらを使う場合は割り込みハンドラ内のEnd of interruptの設定も変更が必要

    // SetIDTEntry(idt[12 + 0x30], CreateIDTAttr(DescriptorType::kInterruptGate, 0),
    //             reinterpret_cast<uint64_t>(InterruptHandlerPS2Mouse), codeSegment);
    // Interrupt_IOAPIC::init();
    // // bootstrap processor APIC ID
    // const uint8_t bspLocalAPICID = *reinterpret_cast<uint32_t *>(0xfee00020) >> 24;

    // Interrupt_IOAPIC::RedirectionTable mouse_tbl = {};
    // mouse_tbl.bits.vector = 0x30 + 12;
    // mouse_tbl.bits.destination = bspLocalAPICID;

    // Interrupt_IOAPIC::addRedirectionTable(12, mouse_tbl);

    // イベントループ
    while (true)
    {
        /* code */
        __asm__("cli");

        if (mainQueue->count() == 0)
        {
            __asm__("sti");
            __asm__("hlt");
        }

        Message msg = mainQueue->get();
        mainQueue->pop();
        __asm__("sti");

        switch (msg.type)
        {
        case Message::InterruptMouse:
            MousePS2::process(*cursor);
            break;

        default:
            printk("EventLoop: Undefined message type: %d", msg.type);
            break;
        }
    }

    Halt();
}
