#include "pic.hpp"

void Interrupt_PIC::endOfInterrupt(uint8_t irq)
{
    outb(PIC1_CMD, EOI); // PIC2からIRQが来た場合はPIC1,2どちらにもEOIを送出

    if (irq >= 8)
        outb(PIC2_CMD, EOI);
}

void Interrupt_PIC::remap(int offset1, int offset2)
{
    outb(PIC1_CMD, ICW1_INIT | ICW1_ICW4); // starts the initialization sequence (in cascade mode)
    io_wait();
    outb(PIC2_CMD, ICW1_INIT | ICW1_ICW4);
    io_wait();
    outb(PIC1_DATA, offset1); // ICW2: Master PIC vector offset
    io_wait();
    outb(PIC2_DATA, offset2); // ICW2: Slave PIC vector offset
    io_wait();
    outb(PIC1_DATA, 1 << CASCADE_IRQ); // ICW3: tell Master PIC that there is a slave PIC at IRQ2
    io_wait();
    outb(PIC2_DATA, 2); // ICW3: tell Slave PIC its cascade identity (0000 0010)
    io_wait();

    outb(PIC1_DATA, ICW4_8086); // ICW4: have the PICs use 8086 mode (and not 8080 mode)
    io_wait();
    outb(PIC2_DATA, ICW4_8086);
    io_wait();

    // mask both PICs.
    outb(PIC1_DATA, 0xff);
    outb(PIC2_DATA, 0xff);
}

void Interrupt_PIC::disableAll(void)
{
    outb(PIC1_DATA, 0xff);
    outb(PIC2_DATA, 0xff);
}

void Interrupt_PIC::disable(uint8_t irqLine)
{
    uint16_t port;
    uint8_t value;

    if (irqLine < 8)
    {
        port = PIC1_DATA;
    }
    else
    {
        port = PIC2_DATA;
        irqLine -= 8;
    }
    value = inb(port) | (1 << irqLine);
    outb(port, value);
}

void Interrupt_PIC::enable(uint8_t irqLine)
{
    uint16_t port;
    uint8_t value;

    if (irqLine < 8)
    {
        port = PIC1_DATA;
    }
    else
    {
        port = PIC2_DATA;
        irqLine -= 8;
    }
    value = inb(port) & ~(1 << irqLine);
    outb(port, value);
}
