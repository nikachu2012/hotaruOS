#include "ioapic.hpp"

extern int printk(const char *c, ...);

uint32_t Interrupt_IOAPIC::read(uint8_t reg)
{
    volatile uint32_t *IND = reinterpret_cast<uint32_t *>(IND_ADDR);
    volatile uint32_t *DAT = reinterpret_cast<uint32_t *>(DAT_ADDR);

    *IND = reg;
    return *DAT;
}

void Interrupt_IOAPIC::write(uint8_t reg, uint32_t data)
{
    volatile uint32_t *IND = reinterpret_cast<uint32_t *>(IND_ADDR);
    volatile uint32_t *DAT = reinterpret_cast<uint32_t *>(DAT_ADDR);

    *IND = reg;
    *DAT = data;
}

void Interrupt_IOAPIC::init()
{
    auto ID_REG = read(0x00);
    auto VER_REG = read(0x01);
    printk("I/O APIC ID: 0x%x, VER: 0x%02x", (ID_REG >> 24) & 0xf, VER_REG & 0xff);

    auto MRE = (VER_REG >> 16) & 0xff; // maximum redirection entries
    printk("Max Redirection Entries: %d", MRE);

    // Disable all interrupts.
    RedirectionTable disable = {};
    disable.bits.mask = 1;
    disable.bits.vector = VECTOR_BASE;
    printk("disable data high:%x, low:%x", disable.data.high, disable.data.low);

    for (int i = 0; i <= MRE; i++)
    {
        write(REDIR_TBL0 + 2 * i, disable.data.low);
        write(REDIR_TBL0 + 2 * i + 1, disable.data.high);
        disable.bits.vector++;
    }

    return;
}

void Interrupt_IOAPIC::addRedirectionTable(int index, RedirectionTable &red)
{
    write(REDIR_TBL0 + 2 * index, red.data.low);
    write(REDIR_TBL0 + 2 * index + 1, red.data.high);
}

void Interrupt_IOAPIC::endOfInterrupt(void)
{
    volatile uint32_t *EOIR = reinterpret_cast<uint32_t *>(LAPIC_EOI_ADDR);
    *EOIR = 0;
}
