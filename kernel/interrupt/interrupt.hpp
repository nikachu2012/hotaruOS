#pragma once

#include <cstdint>

constexpr auto END_OF_INTERRUPT_REG = 0xfee000b0u;
constexpr auto IDT_SIZE = 256;

struct InterruptDescriptor;
extern InterruptDescriptor idt[IDT_SIZE];

enum class DescriptorType
{
    kUpper8Bytes = 0,
    kLDT = 2,
    kTSSAvailable = 9,
    kTSSBusy = 11,
    kCallGate = 12,
    kInterruptGate = 14,
    kTrapGate = 15,
};

union InterruptDescriptorAttribute {
    uint16_t data;
    struct
    {
        uint16_t interruptStackTable : 3;
        uint16_t : 5;
        DescriptorType type : 4;
        uint16_t : 1;
        uint16_t descriptorPrivilegeLevel : 2;
        uint16_t present : 1;
    } __attribute__((packed)) bits;
} __attribute__((packed));

struct InterruptDescriptor
{
    uint16_t offsetLow;
    uint16_t segment_selector;
    InterruptDescriptorAttribute attr;
    uint16_t offsetMiddle;
    uint32_t offsetHigh;
    uint32_t reserved;
} __attribute__((packed));

struct InterruptFrame
{
    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
    uint64_t rsp;
    uint64_t ss;
};

struct IDTR {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

void SetIDTEntry(InterruptDescriptor &desc, InterruptDescriptorAttribute attr, uint64_t offset,
                                uint16_t segmentSelector);
InterruptDescriptorAttribute CreateIDTAttr(DescriptorType type, uint8_t descriptorPrivilegeLevel, bool present = true,
                                           uint8_t interruptStackTable = 0);
__attribute__((no_caller_saved_registers)) void endOfInterrupt();

// Assembly Function
extern "C" void LoadIDT(IDTR *idtr);
extern "C" uint16_t GetCodeSegment();
