#pragma once

#include "../native/io.hpp"
#include <cstdint>

namespace Interrupt_PIC
{
constexpr auto PIC1 = 0x20;
constexpr auto PIC2 = 0xA0;

constexpr auto PIC1_CMD = PIC1;
constexpr auto PIC1_DATA = PIC1 + 1;

constexpr auto PIC2_CMD = PIC2;
constexpr auto PIC2_DATA = PIC2 + 1;

constexpr auto EOI = 0x20; /* End of interrupt */

constexpr auto ICW1_ICW4 = 0x01;      /* Indicates that ICW4 will be present */
constexpr auto ICW1_SINGLE = 0x02;    /* Single (cascade) mode */
constexpr auto ICW1_INTERVAL4 = 0x04; /* Call address interval 4 (8) */
constexpr auto ICW1_LEVEL = 0x08;     /* Level triggered (edge) mode */
constexpr auto ICW1_INIT = 0x10;      /* Initialization - required! */

constexpr auto ICW4_8086 = 0x01;       /* 8086/88 (MCS-80/85) mode */
constexpr auto ICW4_AUTO = 0x02;       /* Auto (normal) EOI */
constexpr auto ICW4_BUF_SLAVE = 0x08;  /* Buffered mode/slave */
constexpr auto ICW4_BUF_MASTER = 0x0C; /* Buffered mode/master */
constexpr auto ICW4_SFNM = 0x10;       /* Special fully nested (not) */

constexpr auto CASCADE_IRQ = 2;

void endOfInterrupt(uint8_t irq);
void remap(int offset1 = 0x20, int offset2 = 0x28);
void disableAll(void);
void enable(uint8_t irqLine);
void disable(uint8_t irqLine);
} // namespace Interrupt_PIC
