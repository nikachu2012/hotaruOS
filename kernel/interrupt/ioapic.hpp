#pragma once

#include <cstdint>

namespace Interrupt_IOAPIC
{

union RedirectionTable {
    struct
    {
        uint32_t low;
        uint32_t high;
    } __attribute__((packed)) data;

    // references:
    // https://www.intel.com/content/dam/www/public/us/en/documents/datasheets/9-series-chipset-pch-datasheet.pdf
    // 12.5.7 REDIR_TBL - Redireciton Table Register
    struct
    {
        uint64_t vector : 8;       // Interrupt Vector
        uint64_t deliveryMode : 3; // 000=Fixed, 001=Lowest Priority, 010=SMI
        uint64_t destMode : 1;     // 0=Physical, 1=Logical
        uint64_t deliveryStatus : 1;
        uint64_t pinPolarity : 1; // 0=Active High, 1=Active Low
        uint64_t remoteIrr : 1;
        uint64_t triggerMode : 1; // 0=Edge triggered, 1=Level triggered
        uint64_t mask : 1;        // 0=Enable interrupt, 1=Disable interrupt(Masked)
        uint64_t reserved : 31;
        uint64_t edid : 8;        // Extended Destination ID
        uint64_t destination : 8; // Destination Local APIC ID
    } __attribute__((packed)) bits;
};

constexpr auto IND_ADDR = 0xfec0'0000; // index register
constexpr auto DAT_ADDR = 0xfec0'0010; // data register
// constexpr auto EOIR_ADDR = 0xfec0'0040;

constexpr auto LAPIC_EOI_ADDR = 0xfee0'00b0; // Local APIC End of interrupt address

constexpr auto REDIR_TBL0 = 0x10;  // Redirection Table Index 0 address (Base Address)
constexpr auto VECTOR_BASE = 0x30; // Interrupt Vector Base

inline uint32_t read(uint8_t reg);
inline void write(uint8_t reg, uint32_t data);

void init();
void addRedirectionTable(int index, RedirectionTable &red);

void endOfInterrupt();
} // namespace Interrupt_IOAPIC
