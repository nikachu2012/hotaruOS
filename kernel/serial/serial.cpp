#include "serial.hpp"

void outb(uint16_t port, uint8_t data)
{
    // out %al, %dx
    // al -> dx
    asm("out %b0, %w1"
        :
        : "a"(data), "d"(port));
}

uint8_t inb(uint16_t port)
{
    uint8_t data;

    // in %dx, %al
    // dx -> al
    asm("in %w1, %b0"
        : "=a"(data)
        : "d"(port));

    return data;
}

int serialInit(const uint16_t port)
{
    // https://wiki.osdev.org/Serial_Ports
    outb(port + 1, 0x00); // Disable all interrupts
    outb(port + 3, 0x80); // Enable DLAB (set baud rate divisor)
    outb(port + 0, 0x01); // Set divisor to 1 (lo byte) 115200 / 1 = 115200 baud
    outb(port + 1, 0x00); //                  (hi byte)
    outb(port + 3, 0x03); // 8 bits, no parity, one stop bit
    outb(port + 2, 0xC7); // Enable FIFO, clear them, with 14-byte threshold
    outb(port + 4, 0x0B); // IRQs enabled, RTS/DSR set
    outb(port + 4, 0x1E); // Set in loopback mode, test the serial chip
    outb(port + 0, 0xAE); // Test serial chip (send byte 0xAE and check if serial returns same byte)

    // Check if serial is faulty (i.e: not same byte as sent)
    if (inb(port + 0) != 0xAE)
    {
        return 1;
    }

    // If serial is not faulty set it in normal operation mode
    // (not-loopback with IRQs enabled and OUT#1 and OUT#2 bits enabled)
    outb(port + 4, 0x0F);

    return 0;
}

int isTransmitEmpty(const uint16_t port)
{
    return inb(port + 5) & 0x20;
}

void serialPutChar(const char data, const uint16_t port)
{
    while (isTransmitEmpty(port) == 0)
        ;

    outb(port, data);
}

int serialPutString(const char *str, const uint16_t port)
{
    unsigned long size = 0;

    while (str[size] != '\0')
    {
        serialPutChar(str[size], port);
        size++;
    }

    return size;
}

int serialReceived(const uint16_t port)
{
    return inb(port + 5) & 1;
}

char serialGetChar(const uint16_t port)
{
    while (serialReceived(port) == 0)
        ;

    return inb(port);
}
